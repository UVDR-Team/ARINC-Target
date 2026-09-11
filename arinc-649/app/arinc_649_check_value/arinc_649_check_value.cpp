// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Entry point of the ARINC 649 Check Value Calculation Application.
 **/

#include <arinc_649/CheckValue.hpp>
#include <arinc_649/CheckValueGenerator.hpp>
#include <arinc_649/CheckValueTypeDescription.hpp>
#include <arinc_649/Version.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Application Entry Point.
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   Arguments
 *
 * @return Application exit status.
 **/
int main( int argc, char * argv[] );

int main( const int argc, char * argv[] )
{
  spdlog::set_level( spdlog::level::level_enum::warn );

  try
  {
    boost::program_options::options_description optionsDescription{ "ARINC 649 Check Value Generator Options" };

    std::vector< std::filesystem::path > filenames;
    std::vector< Arinc649::CheckValueType > checkValueTypes;

    optionsDescription.add_options()
    (
      "help,h",
      "print this help screen"
    )
    (
      "version,v",
      "print version information"
    )
    (
      "filename,f",
      boost::program_options::value( &filenames )
        ->value_name( "filename" )
        ->required()
        ->composing(),
      "filename for calculating check value"
    )
    (
      "check-value-type,t",
      boost::program_options::value( &checkValueTypes )
        ->value_name( "type" )
        ->required()
        ->composing(),
      "Check Value Type"
    );

    std::cout << "ARINC 649 Check Value Calculator\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::parse_command_line( argc, argv, optionsDescription ),
      variablesMap );

    // NOLINTNEXTLINE( readability-container-contains ):VCPKG/MSVC compiles boost programm_options without C++20 support
    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout
        << "Generates ARINC 615A Download Request File\n"
        << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    // NOLINTNEXTLINE( readability-container-contains ):VCPKG/MSVC compiles boost programm_options without C++20 support
    if ( 0U != variablesMap.count( "version" ) )
    {
      std::cout << Arinc649::Version::VersionInformation << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    for ( const auto &filename : filenames )
    {
      std::cout << "File: " << filename << "\n";

      for ( const auto &checkValueType : checkValueTypes )
      {
        const auto checkValue{ Arinc649::CheckValueGenerator::checkValue( checkValueType, filename ) };

        std::cout << " - Check Value: " << checkValue.format() << "\n";
      }
    }

    return EXIT_SUCCESS;
  }
  catch ( const boost::program_options::error &e )
  {
    std::cerr << std::format(
      "Error parsing command line: {}\n"
      "Enter '{} --help' for command line description.\n",
      e.what(),
      argv[ 0 ] );
    return EXIT_FAILURE;
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Error: {}\n", boost::diagnostic_information( e ) );
    return EXIT_FAILURE;
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Error: {}\n", boost::diagnostic_information( e ) );
    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    std::cerr << "Unknown exception occurred\n";
    return EXIT_FAILURE;
  }
}
