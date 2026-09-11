// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Commands::MediaSetManager::ImportMediaSetXmlCommand.
 **/

#ifndef ARINC_665_COMMANDS_MEDIA_SET_MANAGER_IMPORTMEDIASETXMLCOMMAND_HPP
#define ARINC_665_COMMANDS_MEDIA_SET_MANAGER_IMPORTMEDIASETXMLCOMMAND_HPP

#include <arinc_665_commands/media_set_manager/MediaSetManager.hpp>

#include <arinc_665/utils/Arinc665Xml.hpp>
#include <arinc_665/utils/MediaSetManagerConfiguration.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_665/files/Files.hpp>

#include <commands/Commands.hpp>

#include <boost/program_options.hpp>

#include <boost/optional/optional.hpp>

#include <filesystem>
#include <vector>

namespace Arinc665Commands::MediaSetManager {

/**
 * @brief Import Media Set XML into Media Set Manager %Command.
 *
 * Creates a Media Set based on the given XML description and imports them to the media set manager.
 **/
class ARINC_665_COMMANDS_EXPORT ImportMediaSetXmlCommand
{
  public:
    /**
     * @brief Constructs the Import XML Command.
     **/
    ImportMediaSetXmlCommand();

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
    //! Media Set XML File.
    std::vector< std::filesystem::path > mediaSetXmlFilesV;
    //! Media Set Source Directory.
    std::filesystem::path mediaSetSourceDirectoryV;
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
