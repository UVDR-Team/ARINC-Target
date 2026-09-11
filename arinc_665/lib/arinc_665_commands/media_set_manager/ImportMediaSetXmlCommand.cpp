// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Commands::MediaSetManager::ImportMediaSetXmlCommand.
 **/

#include "ImportMediaSetXmlCommand.hpp"

#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/File.hpp>

#include <arinc_665/utils/MediaSetManager.hpp>
#include <arinc_665/utils/FileCreationPolicyDescription.hpp>
#include <arinc_665/utils/FilesystemMediaSetCompiler.hpp>

#include <arinc_665/SupportedArinc665VersionDescription.hpp>
#include <arinc_665/Arinc665Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <format>
#include <iostream>

namespace Arinc665Commands::MediaSetManager {

ImportMediaSetXmlCommand::ImportMediaSetXmlCommand() :
  optionsDescriptionV{ "Import ARINC 665 Media Set XML Options" }
{
  optionsDescriptionV.add_options()
  (
    "media-set-manager-dir,d",
    boost::program_options::value( &mediaSetManagerDirectoryV )
      ->required()
      ->value_name( "Directory" ),
    "ARINC 665 Media Set Manager directory.\n"
    "Required."
  )
  (
    "check-media-set-manager-integrity,c",
    boost::program_options::value( &checkMediaSetManagerIntegrityV )
      ->default_value( true, "true" )
      ->implicit_value( true, "true" )
      ->value_name( "true|false" ),
    "Check Media Set Manager integrity during initialisation."
  )
  (
    "xml-file",
    boost::program_options::value( &mediaSetXmlFilesV )
      ->required()
      ->composing(),
    "ARINC 665 Media Set XML description.\n"
      "Option can be provided multiple times."
  )
  (
    "source-directory",
    boost::program_options::value( &mediaSetSourceDirectoryV )
      ->required(),
      "ARINC 665 source directory.\n"
      "Required."
  )
  (
    "create-batch-files",
    boost::program_options::value( &createBatchFilesV ),
    Arinc665::Utils::FileCreationPolicyDescription::instance()
      .allValues( "ARINC 665 Batch-files creation policy:" )
      .c_str()
  )
  (
    "create-load-header-files",
    boost::program_options::value( &createLoadHeaderFilesV ),
    Arinc665::Utils::FileCreationPolicyDescription::instance()
      .allValues( "ARINC 665 Load-headers-files creation policy:" )
      .c_str()
  )
  (
    "check-file-integrity",
    boost::program_options::value( &checkFileIntegrityV )
      ->implicit_value( true, "true" )
      ->value_name( "true|false" ),
      "Check file integrity during media set registration.\n"
      "Optional."
  )
  (
    "version",
    boost::program_options::value( &versionV ),
    Arinc665::SupportedArinc665VersionDescription::instance()
      .allValues( "ARINC 665 media set format version:" )
      .c_str()
  );
}

void ImportMediaSetXmlCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "Import ARINC 665 Media Set XML\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    // Media Set Manager
    const auto mediaSetManager{ Arinc665::Utils::MediaSetManager::load(
      mediaSetManagerDirectoryV,
      checkMediaSetManagerIntegrityV,
      std::bind_front( &ImportMediaSetXmlCommand::loadProgress, this ) ) };
    assert( mediaSetManager );

    // iterate over XML files
    for ( const auto &mediaSetXmlFile : mediaSetXmlFilesV )
    {
      std::cout << "Load XML: " << mediaSetXmlFile.string() << "\n";

      // load ARINC 665 XML file
      auto [ mediaSet, filePathMapping ]{ Arinc665::Utils::Arinc665Xml_load( mediaSetXmlFile ) };

      auto compiler{ Arinc665::Utils::FilesystemMediaSetCompiler::create() };
      assert( compiler );

      // set exporter parameters
      const auto &defaults{ mediaSetManager->configuration().defaults };
      compiler
        ->mediaSet( mediaSet )
        .arinc665Version( versionV.value_or( defaults.version ) )
        .createBatchFiles( createBatchFilesV.value_or( defaults.batchFileCreationPolicy ) )
        .createLoadHeaderFiles( createLoadHeaderFilesV.value_or( defaults.loadHeaderFileCreationPolicy ) )
        .sourceBasePath( mediaSetSourceDirectoryV )
        .filePathMapping( std::move( filePathMapping ) )
        .outputBasePath( mediaSetManagerDirectoryV );

      auto mediaSetPaths{ ( *compiler )() };

      mediaSetManager->registerMediaSet( mediaSetPaths, checkFileIntegrityV.value_or( defaults.checkFileIntegrity ) );
      mediaSetManager->saveConfiguration();
    }
  }
  catch ( const boost::program_options::error & )
  {
    // parsing errors are handled by command handler
    throw;
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Operation failed: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Operation failed: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cerr << "Operation failed: UNKNOWN EXCEPTION\n";
  }
}

void ImportMediaSetXmlCommand::help() const
{
  std::cout
    << "Compiles Media Set given by XML descriptions and registers it to the Media Set Manager.\n\n"
    << optionsDescriptionV;
}

void ImportMediaSetXmlCommand::loadProgress(
  std::pair< std::size_t, std::size_t > mediaSet,
  std::string_view partNumber,
  std::pair< Arinc665::MediumNumber, Arinc665::MediumNumber > medium )
{
  std::cout << std::format(
    "Loading: {}/{} {} {}:{}\n",
    mediaSet.first,
    mediaSet.second,
    partNumber,
    static_cast< std::string >( medium.first ),
    static_cast< std::string >( medium.second ) );
}

}
