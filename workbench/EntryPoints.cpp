// SPDX-License-Identifier: MPL-2.0
#include "EntryPoints.h"
#include "DemoConfig.hpp"
#include "Arinc615aTha.hpp"
#include "Arinc615aThaC.h"
#include "TargetDataLoaderConfiguration.hpp"
#include <arinc_615a/files/InitializationFile.hpp>
#include <arinc_615a/find/packets/Packet.hpp>
#include <arinc_checksum/CheckValueGenerator.hpp>
#include <arinc_checksum/CheckValue.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdio>
#include <exception>
#include <span>
#include <string_view>

extern "C" int arinc615aDemo(void)
{
  try {
    Arinc615aTha::TargetDataLoaderConfiguration config;
    config.find.localInterfaceAddress = boost::asio::ip::make_address(ARINC_DEMO_ADDRESS);
    config.dataLoader.localInterfaceAddress = config.find.localInterfaceAddress;
    config.find.findPort = ARINC_DEMO_FIND_PORT;
    config.dataLoader.tftpConfiguration.tftpServerPort = ARINC_DEMO_TFTP_PORT;
    config.findInformation.emplace_back("ARINC", "THA", "1", "ARINC 615A Demo", "UVDR");
    Arinc615aTha::OperationConfiguration operations{boost::property_tree::ptree{}};
    operations.informationOperation.enabled = true;
    operations.informationOperation.targetsHardware.emplace_back(
      "ARINC 615A Demo", "DEMO001",
      Arinc615a::Information::PartNumbers{{"DEMO-PN", "", "Demo software"}});
    config.targets.emplace(ARINC_DEMO_TARGET_ID, operations);
    std::printf("ARINC demo: FIND=%u TFTP=%u target=%s\n",
      ARINC_DEMO_FIND_PORT, ARINC_DEMO_TFTP_PORT, ARINC_DEMO_TARGET_ID);
    return Arinc615aTha::run(config);
  } catch (const std::exception &error) {
    std::printf("ARINC demo could not start: %s\n", error.what());
    return 1;
  } catch (...) {
    std::puts("ARINC demo could not start: unknown error");
    return 1;
  }
}

extern "C" int arinc615aRun(const char *filename) { return arinc615a_tha_run_file(filename); }
extern "C" int arinc615aStop(void) { return arinc615a_tha_request_stop(); }

extern "C" int arinc615aSelfTest(void)
{
  try {
    using namespace Arinc615a::Find::Packets;
    const auto raw = Packet{Opcode::InformationRequest, 1}.encode();
    if (raw.empty()) return 1;
    const Arinc615a::Files::InitializationFile initial{
      Arinc615a::Arinc615aVersion::Arinc615a34,
      Arinc615a::Information::InitializationResponse{
        Arinc615a::OperationAcceptanceStatusCode::OperationAccepted}};
    const auto data = static_cast<ArincSupport::RawData>(initial);
    const Arinc615a::Files::InitializationFile decoded{data};
    if (decoded.response().code() != Arinc615a::OperationAcceptanceStatusCode::OperationAccepted) return 2;
    const std::string_view input{"abc"};
    const auto hash = ArincChecksum::CheckValueGenerator::create(ArincChecksum::CheckValueType::Sha256);
    hash->process(std::as_bytes(std::span{input.data(), input.size()}));
    if (hash->checkValue().toString() != "BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD") return 3;
    std::puts("ARINC self-test PASS (codecs and SHA256; network/storage tested separately)");
    return 0;
  } catch (const std::exception &error) {
    std::printf("ARINC self-test failed: %s\n", error.what());
    return 4;
  } catch (...) { return 5; }
}
