// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief ARINC 665 Media Set Print Application.
 **/

#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/utils/FilesystemMediaSetDecompiler.hpp>
#include <arinc_665/utils/MediaSetPrinter.hpp>
#include <arinc_665/utils/MediaSetDefaults.hpp>

#include <arinc_665/Arinc665Exception.hpp>
#include <arinc_665/Version.hpp>

#include <arinc_649/CheckValue.hpp>

#include <helper/Exception.hpp>
#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <filesystem>
#include <iostream>
#include <format>

//! Media Directories
using Directories = std::vector< std::filesystem::path >;

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

/**
 * @brief Loads the media set from the given directory.
 *
 * @param[in] mediaSetDirectories
 *   Media Set Directories
 * @param[in] checkFileIntegrity
 *   Check File Integrity
 *
 * @return Loaded Media Set.
 **/
static Arinc665::Utils::MediaSetDecompilerResult loadMediaSet(
  const Directories &mediaSetDirectories,
  bool checkFileIntegrity );

int main( const int argc, char * argv[] )
try
{
  spdlog::set_level( spdlog::level::level_enum::warn );

  try
  {
    std::cout << std::format(
      "ARINC 665 Media Set Printer - {}\n",
      Arinc665::Version::VersionInformation );

    boost::program_options::options_description optionsDescription{ "ARINC 665 Media Set Printer Options" };

    // directories which contain the medias
    std::vector< std::filesystem::path > directories;

    // Check File Integrity
    bool checkFileIntegrity{};

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
      "directory",
      boost::program_options::value( &directories)
        ->required()
        ->multitoken()
        ->value_name( "directory" ),
      "media directories (can be passed multiple times)"
    )
    (
      "check-file-integrity,i",
      boost::program_options::value( &checkFileIntegrity )
        ->default_value( Arinc665::Utils::MediaSetDefaults::DefaultCheckFileIntegrity )
        ->value_name( "true|false" ),
      "Check File Integrity during decompilation."
    );

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::parse_command_line( argc, argv, optionsDescription ),
      variablesMap );

    // NOLINTNEXTLINE( readability-container-contains ):VCPKG/MSVC compiles boost programm_options without C++20 support
    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout
        << "Prints the ARINC 665 Media Set located in the given directory\n"
        << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    const auto [ mediaSet, checkValues ]{ loadMediaSet( directories, checkFileIntegrity ) };

    std::cout << "Media Set: \n";
    Arinc665::Utils::MediaSetPrinter_print( *mediaSet, std::cout, "", "  " );

    std::cout << "Check Values: \n";
    Arinc665::Utils::MediaSetPrinter_print( checkValues, std::cout, "", "  " );

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

static Arinc665::Utils::MediaSetDecompilerResult loadMediaSet(
  const Directories &mediaSetDirectories,
  const bool checkFileIntegrity )
{
  Arinc665::Utils::MediaPaths mediaPaths{};
  for ( Arinc665::MediumNumber mediumNumber{}; const auto &mediumDirectory : mediaSetDirectories )
  {
    mediaPaths.try_emplace( mediumNumber, mediumDirectory );
    ++mediumNumber;
  }

  auto decompiler{ Arinc665::Utils::FilesystemMediaSetDecompiler::create() };

  decompiler
    ->checkFileIntegrity( checkFileIntegrity )
    .mediaPaths( std::move( mediaPaths ) );

  return ( *decompiler )();
}
