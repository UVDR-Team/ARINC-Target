// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief ARINC 665 Media Set Compiler Application.
 **/

#include <arinc_665/media/Media.hpp>

#include <arinc_665/utils/Arinc665Xml.hpp>
#include <arinc_665/utils/FilesystemMediaSetCompiler.hpp>
#include <arinc_665/utils/FileCreationPolicyDescription.hpp>
#include <arinc_665/utils/MediaSetDefaults.hpp>

#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/Arinc665Exception.hpp>
#include <arinc_665/SupportedArinc665VersionDescription.hpp>
#include <arinc_665/Version.hpp>

#include <helper/Exception.hpp>
#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/optional/optional.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>

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
    std::cout << std::format( "ARINC 665 Media Set Compiler - {}\n", Arinc665::Version::VersionInformation );

    // Media Set XML file
    std::filesystem::path mediaSetXmlFile;
    // Media Set source directory
    std::filesystem::path mediaSetSourceDirectory;
    // Create batch file policy
    Arinc665::Utils::FileCreationPolicy createBatchFiles;
    // Create Load Header file policy
    Arinc665::Utils::FileCreationPolicy createLoadHeaderFiles;
    // ARINC 665 Version
    Arinc665::SupportedArinc665Version version;
    // Media Set destination directory
    std::filesystem::path mediaSetDestinationDirectory;
    // Media Set name
    std::string mediaSetName;

    boost::program_options::options_description optionsDescription{ "ARINC 665 Media Set Compiler Options" };

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
        } ),
      Helper::SeverityLevelDescription::instance().allLevels().c_str()
    )
    (
      "xml-file,f",
      boost::program_options::value( &mediaSetXmlFile )
        ->required()
        ->value_name( "file"),
      "ARINC 665 media set description XML file."
    )
    (
      "source-directory,s",
      boost::program_options::value( &mediaSetSourceDirectory )
        ->required()
        ->value_name( "directory" ),
      "ARINC 665 source directory."
    )
    (
      "create-batch-files,b",
      boost::program_options::value( &createBatchFiles )
        ->value_name( "creation-policy" )
        ->default_value(
          Arinc665::Utils::MediaSetDefaults::DefaultFileCreationPolicy,
          std::string{ Arinc665::Utils::FileCreationPolicyDescription::instance().name(
            Arinc665::Utils::MediaSetDefaults::DefaultFileCreationPolicy ) } ),
      Arinc665::Utils::FileCreationPolicyDescription::instance().allValues( "Batch-files creation policy:" ).c_str()
    )
    (
      "create-load-header-files,c",
      boost::program_options::value( &createLoadHeaderFiles )
        ->value_name( "creation-policy" )
        ->default_value(
          Arinc665::Utils::MediaSetDefaults::DefaultFileCreationPolicy,
          std::string{ Arinc665::Utils::FileCreationPolicyDescription::instance().name(
            Arinc665::Utils::MediaSetDefaults::DefaultFileCreationPolicy ) } ),
      Arinc665::Utils::FileCreationPolicyDescription::instance().allValues( "Load-Headers-files creation policy:" ).c_str()
    )
    (
      "version,v",
      boost::program_options::value( &version )
        ->value_name( "version" )
        ->default_value(
          Arinc665::Utils::MediaSetDefaults::DefaultVersion,
          std::string{ Arinc665::SupportedArinc665VersionDescription::instance().name(
            Arinc665::Utils::MediaSetDefaults::DefaultVersion ) } ),
      Arinc665::SupportedArinc665VersionDescription::instance()
        .allValues( "ARINC 665 media set format version:{}" )
        .c_str()
    )
    (
      "destination-directory,d",
      boost::program_options::value( &mediaSetDestinationDirectory )
        ->default_value( std::filesystem::current_path() )
        ->value_name( "directory" ),
      "Output directory for ARINC 665 media set."
    )
    (
      "media-set-name,n",
      boost::program_options::value( &mediaSetName )
        ->value_name( "name" ),
      "Media Set Name to use.\n"
      "Is set to part number when not provided."
    );

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::parse_command_line( argc, argv, optionsDescription ),
      variablesMap );

    // NOLINTNEXTLINE( readability-container-contains ):VCPKG/MSVC compiles boost programm_options without C++20 support
    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout
        << "Compiles an ARINC 665 Media Set\n"
        << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    // load ARINC 665 XML file
    auto [ mediaSet, fileMapping ]{ Arinc665::Utils::Arinc665Xml_load( mediaSetXmlFile ) };

    auto compiler{ Arinc665::Utils::FilesystemMediaSetCompiler::create() };

    // set exporter parameters
    compiler
      ->mediaSet( mediaSet )
      .arinc665Version( version )
      .createBatchFiles( createBatchFiles )
      .createLoadHeaderFiles( createLoadHeaderFiles )
      .sourceBasePath( mediaSetSourceDirectory )
      .filePathMapping( fileMapping )
      .outputBasePath( mediaSetDestinationDirectory );

    if ( !mediaSetName.empty() )
    {
      compiler->mediaSetName( mediaSetName );
    }

    const auto &[ mediaSetPath, mediaPaths ]{ ( *compiler )() };

    std::cout << "Created Media Set " << mediaSetName << " in \n";
    for ( const auto &[ mediumNumber, mediumPath ] : mediaPaths )
    {
      std::cout << std::format( " * [{}]: {}\n", mediumNumber, ( mediaSetPath / mediumPath ).string() );
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
catch ( ... )
{
  std::cerr << "Very bad exception\n";
  return EXIT_FAILURE;
}
