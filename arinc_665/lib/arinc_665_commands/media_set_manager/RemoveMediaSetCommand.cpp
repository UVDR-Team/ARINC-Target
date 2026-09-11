// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Commands::MediaSetManager::RemoveMediaSetCommand.
 **/

#include "RemoveMediaSetCommand.hpp"

#include <arinc_665/utils/MediaSetManager.hpp>
#include <arinc_665/utils/FilesystemMediaSetRemover.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <format>
#include <iostream>

namespace Arinc665Commands::MediaSetManager {

RemoveMediaSetCommand::RemoveMediaSetCommand() :
  optionsDescriptionV{ "Remove ARINC 665 Media Set Options" }
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
    "media-set-part-number",
    boost::program_options::value( &mediaSetPartNumberV )
      ->required()
      ->composing(),
    "ARINC 665 Media Set Part Numbers to be deleted.\n"
    "Parameter can be provided multiple times."
  );
}

void RemoveMediaSetCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "Remove ARINC 665 Media Set\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    // Media Set Manager
    const auto mediaSetManager{ Arinc665::Utils::MediaSetManager::load(
      mediaSetManagerDirectoryV,
      checkMediaSetManagerIntegrityV,
      std::bind_front( &RemoveMediaSetCommand::loadProgress, this ) ) };
    assert( mediaSetManager );

    if ( const auto mediaSet{ mediaSetManager->mediaSet( mediaSetPartNumberV ) }; !mediaSet )
    {
      BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception() << Helper::AdditionalInfo{ "Media Set does not exist" } );
    }

    auto mediaSetPaths{ mediaSetManager->deregisterMediaSet( mediaSetPartNumberV ) };
    mediaSetManager->saveConfiguration();

    const auto remover{ Arinc665::Utils::FilesystemMediaSetRemover::create() };
    assert( remover );

    mediaSetPaths.first = mediaSetManagerDirectoryV / mediaSetPaths.first;
    remover->mediaSetPaths( mediaSetPaths );
    ( *remover )();
  }
  catch ( const boost::program_options::error & )
  {
    // Command line parsing errors are handled within command handler
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

void RemoveMediaSetCommand::help() const
{
  std::cout
    << "Remove ARINC 665 Media Set from the Media Set Manager.\n\n"
    << optionsDescriptionV;
}

void RemoveMediaSetCommand::loadProgress(
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
