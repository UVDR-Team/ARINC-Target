// SPDX-License-Identifier: MPL-2.0
// Original desktop entry adapter; excluded from the DKM archive.
#include "Arinc615aTha.hpp"
#include "TargetDataLoaderConfiguration.hpp"
#include <arinc_support/Logging.hpp>
#include <boost/exception/all.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>

int Arinc615aTha::run(const int argc, char *argv[])
{
  ArincSupport::Logging::set_level(ArincSupport::Logging::level::warn);
  if (argc < 1 || !argv || !argv[0])
  {
    std::cerr << "Invalid command line arguments\n";
    return EXIT_FAILURE;
  }
  try
  {
    TargetDataLoaderConfiguration configuration;
    boost::program_options::options_description optionsDescription{"ARINC 615A THA Options"};
    optionsDescription.add_options()("help", "print this help screen");
    optionsDescription.add(configuration.dataLoader.options());
    optionsDescription.add(configuration.find.options());
    std::cout << "ARINC 615A THA\n";
    const auto configFile{std::filesystem::path{argv[0]}.replace_extension(".json")};
    boost::property_tree::ptree properties;
    if (std::filesystem::is_regular_file(configFile))
      boost::property_tree::read_json(configFile.string(), properties);
    else
      ARINC_LOG_WARN("No configuration file '{}' Found. Using default configuration - No operations are active.", configFile.string());
    configuration.fromProperties(properties);
    boost::program_options::variables_map variablesMap;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, optionsDescription), variablesMap);
    if (variablesMap.count("help"))
    {
      std::cout << "ARINC 615A Test Hardware\n" << optionsDescription << "\n";
      return EXIT_FAILURE;
    }
    boost::program_options::notify(variablesMap);
    return detail::runConfigured(configuration, true);
  }
  catch (const boost::program_options::error &e)
  {
    std::cerr << "Error parsing command line: " << e.what() << '\n'
      << "Enter '" << argv[0] << " --help' for command line description.\n";
  }
  catch (const boost::exception &e) { std::cerr << "Error: " << boost::diagnostic_information(e) << '\n'; }
  catch (const std::exception &e) { std::cerr << "Error: " << e.what() << '\n'; }
  catch (...) { std::cerr << "Unknown exception occurred\n"; }
  return EXIT_FAILURE;
}
