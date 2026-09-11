// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Entry point of the ARINC 615A Download Request File Generation Application.
 **/

#include <arinc_615a/files/download/DownloadOperationRequestFile.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/Arinc615aVersionDescription.hpp>
#include <arinc_615a/Version.hpp>

#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
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
try
{
  spdlog::set_level( spdlog::level::level_enum::warn );

  try
  {
    std::cout << std::format(
      "ARINC 615A Download Request File Generator - {}\n",
      Arinc615a::Version::VersionInformation );

    boost::program_options::options_description optionsDescription{
      "ARINC 615A Download Request File Generator Options" };

    auto const &versionDes{ Arinc615a::Arinc615aVersionDescription::instance() };

    const std::string versionValues{
      " - '"
      + std::string{ versionDes.name( Arinc615a::Arinc615aVersion::Arinc615a2 ) }
      +   "': ARINC 615A-2\n"

      + " - '"
      + std::string{ versionDes.name( Arinc615a::Arinc615aVersion::Arinc615a34 ) }
      +  "': ARINC 615A-3/ 615A-4" };

    std::filesystem::path filename;
    std::vector< std::string > downloadFiles;
    std::string userDefinedData;
    Arinc615a::Arinc615aVersion version;

    optionsDescription.add_options()
    (
      "help,h",
      "print this help screen"
    )
    (
      "log-level,l",
      boost::program_options::value< spdlog::level::level_enum >()
        ->default_value( spdlog::level::level_enum::warn, "warn" )
        ->value_name( "log-level" )
        ->notifier( []( const auto &logLevel ) {
          spdlog::set_level( logLevel );
        }),
      Helper::SeverityLevelDescription::instance().allLevels().c_str()
    )
    (
      "filename,f",
      boost::program_options::value( &filename )
        ->value_name( "filename" )
        ->required(),
      "ARINC 615A Download Request File filename."
    )
    (
      "download-file",
      boost::program_options::value( &downloadFiles )
        ->value_name( "filename" )
        ->required()
        ->multitoken(),
      "Filename to be added to the ARINC 615A Download Request File."
    )
    (
      "user-defined-data",
      boost::program_options::value( &userDefinedData )
        ->value_name( "data" ),
      "Adds the User Defined Data to the ARINC 615A Download Request File."
    )
    (
      "arinc615a-version",
      boost::program_options::value( &version )
        ->value_name( "version" )
        ->default_value( Arinc615a::Arinc615aVersion::Arinc615a2 ),
      ( std::string{ "ARINC 615A Version:\n" } + versionValues ).c_str()
    );

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::parse_command_line( argc, argv, optionsDescription ),
      variablesMap );

    // NOLINTNEXTLINE( readability-container-contains ):VCPKG/MSVC compiles boost programm_options without C++20 support
    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout << "Generates ARINC 615A Download Request File\n" << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    std::cout << "Generate: " << filename << "\n";

    Arinc615a::Files::DownloadOperationRequestFile requestFile{ version };

    for ( auto &file : downloadFiles )
    {
      requestFile.file( std::move( file ) );
    }

    auto userDefinedDataSpan{ std::as_bytes( std::span{ userDefinedData.data(), userDefinedData.size() } ) };
    requestFile.userDefinedData( Helper::RawData{ userDefinedDataSpan.begin(), userDefinedDataSpan.end() } );

    std::fstream outFileStream{ filename, std::fstream::out | std::fstream::trunc | std::fstream::binary };

    const auto data{ static_cast< Helper::RawData >( requestFile ) };
    outFileStream.write(
      reinterpret_cast< char const * >( data.data() ),
      static_cast< std::streamsize >( data.size() ) );

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
catch ( ... )
{
  std::cerr << "Very bad exception\n";
  return EXIT_FAILURE;
}
