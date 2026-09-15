// SPDX-License-Identifier: MPL-2.0
#include "Arinc615aThaLib.hpp"
#include <arinc_615a/find/packets/Packet.hpp>
#include <arinc_615a/files/InitializationFile.hpp>
#include <arinc_615a/information/InitializationResponse.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/version.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <thread>
#include <atomic>

using boost::asio::ip::udp;
using namespace std::chrono_literals;
static void require(bool condition, const char *message)
{
  if (!condition) throw std::runtime_error(message);
}
static unsigned short freePort(boost::asio::io_context &io)
{
  udp::socket socket(io, {boost::asio::ip::address_v4::loopback(), 0});
  return socket.local_endpoint().port();
}
static Helper::RawData receive(udp::socket &socket)
{
  Helper::RawData data(4096);
  udp::endpoint from;
  socket.non_blocking(true);
  const auto deadline = std::chrono::steady_clock::now() + 2s;
  while (std::chrono::steady_clock::now() < deadline)
  {
    boost::system::error_code ec;
    const auto n = socket.receive_from(boost::asio::buffer(data), from, 0, ec);
    if (!ec) { data.resize(n); return data; }
    if (ec != boost::asio::error::would_block && ec != boost::asio::error::try_again)
      throw boost::system::system_error(ec);
    std::this_thread::sleep_for(1ms);
  }
  throw std::runtime_error("UDP response timed out");
}
struct RunningTarget
{
  std::thread worker;
  std::atomic<int> result{99};
  explicit RunningTarget(bool background)
  {
    if (background) require(arinc615a_tha_start(1) == 0, "background start failed");
    else worker = std::thread([this] { result = arinc615a_tha_start(0); });
    const auto deadline = std::chrono::steady_clock::now() + 2s;
    while (!arinc615a_tha_is_running() && std::chrono::steady_clock::now() < deadline)
      std::this_thread::sleep_for(1ms);
  }
  ~RunningTarget()
  {
    arinc615a_tha_stop();
    if (worker.joinable()) worker.join();
  }
};
int main()
{
  const auto directory = std::filesystem::temp_directory_path() /
    ("arinc615a-regression-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
  std::filesystem::create_directory(directory);
  try
  {
    arinc615a_tha_set_log_level(ARINC615A_LOG_OFF);
    require(arinc615a_tha_init_json("{") == ARINC615A_ERROR, "invalid JSON accepted");
    boost::asio::io_context io;
    const auto findPort = freePort(io);
    auto tftpPort = freePort(io);
    while (tftpPort == findPort) tftpPort = freePort(io);
    boost::property_tree::ptree config;
    config.put("arinc_615a.local_tftp_address", "127.0.0.1");
    config.put("arinc_615a.tftp.port", tftpPort);
    config.put("arinc_615a.protocol_file_logging", false);
    config.put("arinc_615a_find.local_find_address", "127.0.0.1");
    config.put("arinc_615a_find.find_port", findPort);
    boost::property_tree::ptree target, targets, find, finds;
    target.put("target_id", "ABCDEF_123");
    target.put("upload_operation.enabled", true);
    target.put("upload_operation.directory", directory.string());
    targets.push_back({"", target});
    config.add_child("targets_configuration", targets);
    find.put("thwId", "ABCDEF");
    find.put("thwPosition", "123");
    find.put("thwTypeName", "TEST");
    find.put("literalName", "TEST");
    find.put("manufacturerCode", "UVR");
    finds.push_back({"", find});
    config.add_child("find_information", finds);
    std::ostringstream json;
    boost::property_tree::write_json(json, config);

#if BOOST_VERSION < 108800
    auto unsupported = config;
    auto &entry = unsupported.get_child("targets_configuration").front().second;
    entry.put("information_operation.enabled", true);
    entry.put("information_operation.list_check_value", "SHA256");
    require(Arinc615aTha::init(unsupported) == ARINC615A_ERROR, "unsupported hash accepted");
#endif
    for (int iteration = 0; iteration < 12; ++iteration)
    {
      require(arinc615a_tha_init_json(json.str().c_str()) == 0, "init failed");
      bool background = false;
#ifdef ARINC615A_BACKGROUND_THREAD
      background = iteration % 2;
#else
      require(arinc615a_tha_start(1) == ARINC615A_UNSUPPORTED, "disabled background accepted");
#endif
      {
        RunningTarget running(background);
        require(arinc615a_tha_is_running(), "target did not start");
        require(arinc615a_tha_start(0) == ARINC615A_ALREADY_RUN, "duplicate start accepted");
        require(arinc615a_tha_init_default() == ARINC615A_ALREADY_RUN, "live reconfiguration accepted");
        udp::socket socket(io, {boost::asio::ip::address_v4::loopback(), 0});
        const Arinc615a::Find::Packets::Packet irq(
          Arinc615a::Find::Packets::Opcode::InformationRequest,
          Arinc615a::Find::Packets::Packet::Parameters{""});
        const auto raw = irq.encode();
        socket.send_to(boost::asio::buffer(raw), {boost::asio::ip::address_v4::loopback(), findPort});
        const auto reply = receive(socket);
        const Arinc615a::Find::Packets::Packet answer(reply);
        require(answer.parameter(0) == "ABCDEF", "incorrect FIND response");

        const Tftp::Packets::ReadRequestPacket missing("MISSING_123.LUI", Tftp::Packets::TransferMode::OCTET, {});
        const auto rawMissing = static_cast<Helper::RawData>(missing);
        for (int i = 0; i < 2; ++i)
          socket.send_to(boost::asio::buffer(rawMissing), {boost::asio::ip::address_v4::loopback(), tftpPort});
        for (int i = 0; i < 2; ++i)
        {
          const auto deniedRaw = receive(socket);
          const Tftp::Packets::DataPacket denied(deniedRaw);
          const Arinc615a::Files::InitializationFile deniedFile(denied.data());
          require(deniedFile.response().code() == Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
            "unknown target accepted");
        }

        // Verify that an existing upload directory is accepted on the wire.
        const Tftp::Packets::ReadRequestPacket request("ABCDEF_123.LUI", Tftp::Packets::TransferMode::OCTET, {});
        const auto rawRequest = static_cast<Helper::RawData>(request);
        socket.send_to(boost::asio::buffer(rawRequest), {boost::asio::ip::address_v4::loopback(), tftpPort});
        const auto rawResponse = receive(socket);
        const Tftp::Packets::DataPacket response(rawResponse);
        const Arinc615a::Files::InitializationFile initialization(response.data());
        require(initialization.response().code() == Arinc615a::OperationAcceptanceStatusCode::OperationAccepted,
          "existing upload directory was rejected");
        // Stop with an active transfer and outstanding receive/timer callbacks.
      }
      require(!arinc615a_tha_is_running(), "stop did not finish");
      arinc615a_tha_stop();
    }
    // Failed socket binding must clean up and permit a later restart.
    {
      udp::socket occupied(io, {boost::asio::ip::address_v4::loopback(), findPort});
      require(arinc615a_tha_start(0) == ARINC615A_ERROR, "occupied port accepted");
    }
    { RunningTarget running(false); require(arinc615a_tha_is_running(), "restart after failure failed"); }
    std::filesystem::remove_all(directory);
    std::cout << "Lifecycle, restart, FIND and upload acceptance passed\n";
    return 0;
  }
  catch (const std::exception &e)
  {
    arinc615a_tha_stop();
    std::filesystem::remove_all(directory);
    std::cerr << e.what() << '\n';
    return 1;
  }
}
