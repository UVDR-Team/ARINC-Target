// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Commands::MediaSetManager::ListLoadsCommand.
 **/

#ifndef ARINC_665_COMMANDS_MEDIA_SET_MANAGER_LISTLOADSCOMMAND_HPP
#define ARINC_665_COMMANDS_MEDIA_SET_MANAGER_LISTLOADSCOMMAND_HPP

#include <arinc_665_commands/media_set_manager/MediaSetManager.hpp>

#include <arinc_665/Arinc665.hpp>

#include <commands/Commands.hpp>

#include <boost/program_options.hpp>

#include <filesystem>

namespace Arinc665Commands::MediaSetManager {

/**
 * @brief List Loads within Media Set Manager %Command.
 *
 * List all loads, which are part of the media sets of the Media Set Manager.
 **/
class ARINC_665_COMMANDS_EXPORT ListLoadsCommand
{
  public:
    /**
     * @brief Constructs the List Loads Command.
     **/
    ListLoadsCommand();

    /**
     * @brief Executes the Operation.
     *
     * @param[in] parameters
     *   Command parameters.
     **/
    void execute( const Commands::Parameters &parameters );

    //! Prints command help.
    void help() const;

  private:
    /**
     * @brief Load progress indicator.
     *
     * @param[in] mediaSet
     *   Media Set information
     * @param[in] partNumber
     *   Media Set Part Number
     * @param[in] medium
     *   Medium information
     **/
    void loadProgress(
      std::pair< std::size_t, std::size_t > mediaSet,
      std::string_view partNumber,
      std::pair< Arinc665::MediumNumber, Arinc665::MediumNumber > medium );

    //! Program Options Description.
    boost::program_options::options_description optionsDescriptionV;
    //! Media Set Manager Directory.
    std::filesystem::path mediaSetManagerDirectoryV;
    //! Check Media Set Manager Integrity during Initialisation.
    bool checkMediaSetManagerIntegrityV{ true };
};

}

#endif
