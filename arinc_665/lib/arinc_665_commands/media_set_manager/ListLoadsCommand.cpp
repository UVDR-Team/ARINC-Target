// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Commands::MediaSetManager::ListLoadsCommand.
 **/

#include "ListLoadsCommand.hpp"

#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/Load.hpp>

#include <arinc_665/utils/MediaSetManager.hpp>

#include <boost/exception/all.hpp>

#include <format>
#include <iostream>

namespace Arinc665Commands::MediaSetManager {

ListLoadsCommand::ListLoadsCommand() :
  optionsDescriptionV{ "List ARINC 665 Loads Options" }
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
  );
}

void ListLoadsCommand::execute( const Commands::Parameters &parameters )
{
  try
  {
    std::cout << "List ARINC 665 Loads\n";

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::command_line_parser( parameters ).options( optionsDescriptionV ).run(),
      variablesMap );
    boost::program_options::notify( variablesMap );

    // Media Set Manager
    const auto mediaSetManager{ Arinc665::Utils::MediaSetManager::load(
      mediaSetManagerDirectoryV,
      checkMediaSetManagerIntegrityV,
      std::bind_front( &ListLoadsCommand::loadProgress, this ) ) };
    assert( mediaSetManager );

    if ( const auto loads{ mediaSetManager->loads() }; loads.empty() )
    {
      std::cout << "*** No loads within media set manger ***\n";
    }
    else
    {
      for ( const auto &load : loads )
      {
        std::cout << std::format(
          "Media Set P/N:        {}\n"
          "Load Header Filename: {}\n"
          "Load P/N:             {}\n",
          load->mediaSet()->partNumber(),
          load->name(),
          load->partNumber() );

        if ( const auto loadType{ load->loadType() }; loadType )
        {
          std::cout << std::format( "Load Type:            {} (0x{:08X})\n", loadType->first, loadType->second );
        }

        std::cout << "\n";
      }
    }
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

void ListLoadsCommand::help() const
{
  std::cout
    << "List all loads contained with the Media Set Manager.\n\n"
    << optionsDescriptionV;
}

void ListLoadsCommand::loadProgress(
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
