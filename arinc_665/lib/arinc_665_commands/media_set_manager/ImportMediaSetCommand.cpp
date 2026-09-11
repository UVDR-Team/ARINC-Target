// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Commands::MediaSetManager::ImportMediaSetCommand.
 **/

#include "ImportMediaSetCommand.hpp"

#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/files/MediaSetInformation.hpp>

#include <arinc_665/utils/MediaSetManager.hpp>
#include <arinc_665/utils/FilesystemMediaSetDecompiler.hpp>
#include <arinc_665/utils/FilesystemMediaSetCopier.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <format>
#include <iostream>

namespace Arinc665Commands::MediaSetManager {

ImportMediaSetCommand::ImportMediaSetCommand() :
  optionsDescriptionV{ "Import ARINC 665 Media Set Options" }
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
    "source-directory",
    boost::program_options::value( &mediaSourceDirectoriesV )
      ->required()
      ->composing(),
    "ARINC 665 media source directories.\n"
      "Must be provided for each media directory."
  )
  (
    "check-file-integrity",
    boost::program_options::value( &checkFileIntegrityV )
      ->implicit_value( true, "true" )
      ->value_name( "true|false" ),
    "Check file integrity during media set importing and registration.\n"
    "Optional."
  );
}

void ImportMediaSetCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "Import ARINC 665 Media Set\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    // Media Set Manager
    const auto mediaSetManager{ Arinc665::Utils::MediaSetManager::load(
      mediaSetManagerDirectoryV,
      checkMediaSetManagerIntegrityV,
      std::bind_front( &ImportMediaSetCommand::loadProgress, this ) ) };

    // Fill the media paths list
    Arinc665::Utils::MediaPaths sourceMediaPaths{};
    for ( const auto &mediumSourceDirectory : mediaSourceDirectoriesV )
    {
      const auto mediumInformation{ Arinc665::Utils::getMediumInformation( mediumSourceDirectory ) };

      if ( !mediumInformation )
      {
        BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value{ mediumSourceDirectory } );
      }

      sourceMediaPaths.try_emplace( mediumInformation->mediaSequenceNumber, mediumSourceDirectory );
    }

    auto importer{ Arinc665::Utils::FilesystemMediaSetDecompiler::create() };
    assert( importer );

    const auto &defaults{ mediaSetManager->configuration().defaults };

    importer
      ->checkFileIntegrity( checkFileIntegrityV.value_or( defaults.checkFileIntegrity ) )
      .mediaPaths( sourceMediaPaths );

    const auto &[ mediaSet, checkValues]{ ( *importer )() };

    if ( mediaSetManager->hasMediaSet( mediaSet->partNumber() ) )
    {
      BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{} << Helper::AdditionalInfo{ "Media Set already exist" } );
    }

    const auto copier{ Arinc665::Utils::FilesystemMediaSetCopier::create() };
    assert( copier );

    copier
      ->mediaPaths( sourceMediaPaths )
      .outputBasePath( mediaSetManagerDirectoryV )
      .mediaSetName( std::string{ mediaSet->partNumber() } );

    auto destinationPaths{ ( *copier )() };

    mediaSetManager->registerMediaSet(
      { std::move( destinationPaths ) },
      checkFileIntegrityV.value_or( defaults.checkFileIntegrity ) );

    mediaSetManager->saveConfiguration();
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

void ImportMediaSetCommand::help() const
{
  std::cout
    << "Import existing ARINC 665 Media Set into Media Set Manager.\n\n"
    << optionsDescriptionV;
}

void ImportMediaSetCommand::loadProgress(
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
