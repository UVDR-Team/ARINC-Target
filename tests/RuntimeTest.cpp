// SPDX-License-Identifier: MPL-2.0
#include "Arinc615aTha.hpp"
#include "Arinc615aThaC.h"
#include "TargetDataLoaderConfiguration.hpp"
#include <arinc_615a/find/packets/Packet.hpp>
#include <arinc_615a/files/InitializationFile.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/test/unit_test.hpp>
#include <array>
#include <chrono>
#include <future>
#include <thread>

extern "C" int arinc_c_interface_smoke(void);
using namespace std::chrono_literals;
using Udp = boost::asio::ip::udp;

namespace {
unsigned short unusedPort()
{
  boost::asio::io_context io;
  Udp::socket socket{io, {boost::asio::ip::address_v4::loopback(), 0}};
  return socket.local_endpoint().port();
}

struct ServerTask
{
  std::future<int> result;
  explicit ServerTask(Arinc615aTha::TargetDataLoaderConfiguration config) :
    result{std::async(std::launch::async, [config] { return Arinc615aTha::run(config); })} {}
  ~ServerTask()
  {
    Arinc615aTha::requestStop();
    if (result.valid()) result.wait();
  }
};

std::vector<std::byte> receive(Udp::socket &socket, Udp::endpoint &from, std::chrono::milliseconds timeout)
{
  std::array<std::byte, 2048> bytes{};
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  while (std::chrono::steady_clock::now() < deadline)
  {
    boost::system::error_code error;
    const auto n = socket.receive_from(boost::asio::buffer(bytes), from, 0, error);
    if (!error) return {bytes.begin(), bytes.begin() + n};
    if (error != boost::asio::error::would_block && error != boost::asio::error::try_again)
      throw boost::system::system_error{error};
    std::this_thread::sleep_for(2ms);
  }
  return {};
}

void ready(Udp::socket &socket, const Udp::endpoint &server, const Arinc615aTha::TargetDataLoaderConfiguration &config)
{
  using namespace Arinc615a::Find::Packets;
  const auto request = Packet{Opcode::InformationRequest, 1}.encode();
  Udp::endpoint from;
  for (int attempt = 0; attempt < 100; ++attempt)
  {
    socket.send_to(boost::asio::buffer(request), server);
    const auto answer = receive(socket, from, 20ms);
    if (answer.empty()) continue;
    const auto &info = config.findInformation.front();
    const auto expected = Packet{Opcode::InformationAnswer,
      Packet::Parameters{info.thwId, info.thwTypeName, info.thwPosition, info.literalName, info.manufacturerCode}}.encode();
    BOOST_REQUIRE(answer == expected);
    return;
  }
  BOOST_FAIL("FIND server did not respond within two seconds");
}

Arinc615a::Files::InitializationFile requestInitialization(const Udp::endpoint &server, const std::string &filename)
{
  boost::asio::io_context io;
  Udp::socket socket{io, {boost::asio::ip::address_v4::loopback(), 0}};
  socket.non_blocking(true);
  // RFC 1350 RRQ in octet mode, without optional extensions.
  std::string request{"\0\1", 2};
  request += filename;
  request.append("\0octet\0", 7);
  socket.send_to(boost::asio::buffer(request), server);
  Udp::endpoint from;
  const auto answer = receive(socket, from, 2000ms);
  BOOST_REQUIRE_GE(answer.size(), 4U);
  BOOST_REQUIRE(answer[0] == std::byte{0} && answer[1] == std::byte{3});
  BOOST_REQUIRE(answer[2] == std::byte{0} && answer[3] == std::byte{1});
  const std::array<unsigned char, 4> ack{0, 4, 0, 1};
  socket.send_to(boost::asio::buffer(ack), from);
  std::this_thread::sleep_for(10ms);
  return Arinc615a::Files::InitializationFile{ArincSupport::ConstRawDataSpan{answer}.subspan(4)};
}
}

BOOST_AUTO_TEST_CASE(c_wrapper_rejects_invalid_arguments)
{
  BOOST_CHECK(arinc_c_interface_smoke());
  BOOST_CHECK(!Arinc615aTha::requestStop());
  BOOST_CHECK_NE(arinc615a_tha_run_file("/nonexistent/arinc-missing-config.json"), 0);
}

BOOST_AUTO_TEST_CASE(server_restart_find_and_all_four_operation_rejections)
{
  Arinc615aTha::TargetDataLoaderConfiguration config;
  config.find.localInterfaceAddress = boost::asio::ip::address_v4::loopback();
  config.dataLoader.localInterfaceAddress = boost::asio::ip::address_v4::loopback();
  config.find.findPort = unusedPort();
  do { config.dataLoader.tftpConfiguration.tftpServerPort = unusedPort(); }
  while (config.find.findPort == config.dataLoader.tftpConfiguration.tftpServerPort);
  config.findInformation.emplace_back("TEST", "THA", "1", "Offline test", "TST");
  config.targets.emplace("TEST_1", Arinc615aTha::OperationConfiguration{boost::property_tree::ptree{}});
  const Udp::endpoint findEndpoint{config.find.localInterfaceAddress, config.find.findPort};
  const Udp::endpoint tftpEndpoint{config.dataLoader.localInterfaceAddress, config.dataLoader.tftpConfiguration.tftpServerPort};
  for (int cycle = 0; cycle < 3; ++cycle)
  {
    ServerTask server{config};
    boost::asio::io_context io;
    Udp::socket probe{io, {boost::asio::ip::address_v4::loopback(), 0}};
    probe.non_blocking(true);
    ready(probe, findEndpoint, config);
    BOOST_CHECK_NE(Arinc615aTha::run(config), 0); // One active server at a time.
    for (const auto *extension : {".LCI", ".LUI", ".LND", ".LNO"})
    {
      const auto file = requestInitialization(tftpEndpoint, std::string{"TEST_1"} + extension);
      BOOST_CHECK(file.response().code() == Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported);
      BOOST_CHECK(file.protocolVersion() == config.version);
    }
    const auto denied = requestInitialization(tftpEndpoint, "UNKNOWN_1.LCI");
    BOOST_CHECK(denied.response().code() == Arinc615a::OperationAcceptanceStatusCode::OperationDenied);
    BOOST_CHECK(denied.response().description() == "Invalid Target ID");
    BOOST_REQUIRE(Arinc615aTha::requestStop());
    BOOST_REQUIRE(server.result.wait_for(3s) == std::future_status::ready);
    BOOST_CHECK_EQUAL(server.result.get(), 0);
    BOOST_CHECK(!Arinc615aTha::requestStop());
  }
}
