// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Commands::MediaSetManager::CreateMediaSetManagerCommand.
 **/

#ifndef ARINC_665_COMMANDS_MEDIA_SET_MANAGER_CREATEMEDIASETMANAGERCOMMAND_HPP
#define ARINC_665_COMMANDS_MEDIA_SET_MANAGER_CREATEMEDIASETMANAGERCOMMAND_HPP

#include <arinc_665_commands/media_set_manager/MediaSetManager.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <commands/Commands.hpp>

#include <boost/program_options.hpp>

#include <boost/optional/optional.hpp>

#include <filesystem>

namespace Arinc665Commands::MediaSetManager {

/**
 * @brief Create Media Set Manager %Command.
 *
 * Creates a Media Set Manager Configuration with a specified directory.
 **/
class ARINC_665_COMMANDS_EXPORT CreateMediaSetManagerCommand
{
  public:
    /**
     * @brief Constructs the Command.
     **/
    CreateMediaSetManagerCommand();

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
    //! Program Options Description.
    boost::program_options::options_description optionsDescriptionV;
    //! Media Set Manager Directory.
    std::filesystem::path mediaSetManagerDirectoryV;
    //! Create Batch File Policy.
    boost::optional< Arinc665::Utils::FileCreationPolicy > createBatchFilesV;
    //! Create Load Header File Policy.
    boost::optional< Arinc665::Utils::FileCreationPolicy > createLoadHeaderFilesV;
    //! ARINC 665 Version.
    boost::optional< Arinc665::SupportedArinc665Version > versionV;
    //! Check File Integrity.
    boost::optional< bool > checkFileIntegrityV;
};

}

#endif
