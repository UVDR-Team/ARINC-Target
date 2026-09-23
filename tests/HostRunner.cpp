// SPDX-License-Identifier: MPL-2.0
#include "Arinc615aThaC.h"
#include "../workbench/EntryPoints.h"
#include <arinc_665/files/LoadHeaderFile.hpp>
#include <arinc_checksum/Arinc645Crc.hpp>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <thread>

int main(int argc, char **argv)
{
  if (argc == 2 && std::string_view(argv[1]) == "--self-test") return arinc615aSelfTest();
  if (argc == 3 && std::string_view(argv[1]) == "--fixtures") {
    const std::filesystem::path dir{argv[2]};
    std::filesystem::create_directories(dir);
    ArincSupport::RawData payload(4097);
    for (std::size_t i = 0; i < payload.size(); ++i) payload[i] = std::byte(i % 251);
    ArincChecksum::Arinc645Crc16 crc;
    crc.process_bytes(payload.data(), payload.size());
    Arinc665::Files::LoadHeaderFile header;
    header.partNumber("DEMO-PN");
    header.dataFiles().push_back({"payload.bin", "DEMO-PN", payload.size(), crc.checksum(), {}});
    const auto bytes = static_cast<ArincSupport::RawData>(header);
    header.dataFiles().clear();
    const auto empty = static_cast<ArincSupport::RawData>(header);
    header.dataFiles().push_back({"../escape.bin", "DEMO-PN", payload.size(), crc.checksum(), {}});
    const auto traversal = static_cast<ArincSupport::RawData>(header);
    for (const auto &[name, data] : {std::pair{"demo.LUH", bytes}, std::pair{"payload.bin", payload},
        std::pair{"empty.LUH", empty}, std::pair{"traversal.LUH", traversal}}) {
      std::ofstream file{dir / name, std::ios::binary};
      file.write(reinterpret_cast<const char *>(data.data()), data.size());
      if (!file) return 2;
    }
    return 0;
  }
  if (argc != 2) { std::cerr << "Usage: arinc_host_runner config.json\n"; return 2; }
  auto result = std::async(std::launch::async, [&] { return arinc615a_tha_run_file(argv[1]); });
  std::string command;
  std::getline(std::cin, command);
  for (int attempt = 0; attempt < 100 && result.wait_for(std::chrono::milliseconds{0}) != std::future_status::ready; ++attempt) {
    arinc615a_tha_request_stop();
    std::this_thread::sleep_for(std::chrono::milliseconds{10});
  }
  return result.get();
}
