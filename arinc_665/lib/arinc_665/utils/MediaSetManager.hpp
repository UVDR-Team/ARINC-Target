// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::MediaSetManager.
 **/

#ifndef ARINC_665_UTILS_MEDIASETMANAGER_HPP
#define ARINC_665_UTILS_MEDIASETMANAGER_HPP

#include <arinc_665/utils/Utils.hpp>
#include <arinc_665/utils/MediaSetManagerConfiguration.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_649/CheckValue.hpp>

#include <filesystem>
#include <functional>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <string_view>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Manager.
 *
 * This class manages the locally held media sets.
 *
 * @par Organisation of Media Set Manager data.
 * - The configuration is held within a JSON file within the media set directory.
 * - Media sets are stored beneath the media set directory.
 * - Within this directory the media sets each are stored within a directory named @p mediaSetName.
 * - Within the media set directory the media are stored with the corresponding medium-path mapping.
 **/
class ARINC_665_EXPORT MediaSetManager
{
  public:
    /**
     * @brief Load Media Set Manager Progress Handler.
     *
     * This handler is called during initialisation of the *Media Set Manager* to indicate progress information.
     *
     * @param[in] mediaSet
     *   A @p std::pair of the current media set and number of media sets.
     * @param[in] partNumber
     *   Media Set Part Number
     * @param[in] medium
     *   A @p std::pair of the current medium and number of media.
     **/
    using LoadProgressHandler =
      std::function< void(
        std::pair< std::size_t, std::size_t > mediaSet,
        std::string_view partNumber,
        std::pair< MediumNumber, MediumNumber > medium ) >;

    //! Media Set Manager Configuration Filename
    static constexpr std::string_view ConfigurationFilename{ "MediaSetManager.json" };

    /**
     * @brief Creates an empty Media Set Manager.
     *
     * @param[in] directory
     *   Directory for Media Set Manger.
     *   Must not exist.
     *
     * @return Media Set Manager Instance.
     **/
    [[nodiscard]] static MediaSetManagerPtr create( std::filesystem::path directory );

    /**
     * @brief Loads the Media Set Configuration.
     *
     * @param[in] directory
     *   Directory for Media Set Manger.
     * @param[in] checkFileIntegrity
     *   If set to @p true, additional file integrity checks are performed.
     * @param[in] loadProgressHandler
     *   Handler for load progress.
     *
     * @return Media Set Manager Instance.
     **/
    [[nodiscard]] static MediaSetManagerPtr load(
      std::filesystem::path directory,
      bool checkFileIntegrity = true,
      const LoadProgressHandler &loadProgressHandler = {} );

    /**
     * @brief Checks if a Media Set Manager Configuration is available or creates it.
     *
     * @param[in] directory
     *   Directory for Media Set Manger.
     * @param[in] checkFileIntegrity
     *   If set to @p true, additional file integrity checks are performed
     * @param[in] loadProgressHandler
     *   Handler for load progress.
     *
     * @return Media Set Manager
     **/
    [[nodiscard]] static MediaSetManagerPtr loadOrCreate(
      std::filesystem::path directory,
      bool checkFileIntegrity = true,
      const LoadProgressHandler &loadProgressHandler = {} );

    //! Destructor
    virtual ~MediaSetManager() = default;

    /**
     * @name Media Set Defaults
     * @{
     **/

    /**
     * @brief Returns the Media Set Defaults
     *
     * @return Media Set Defaults
     **/
    [[nodiscard]] virtual const MediaSetDefaults& mediaSetDefaults() const = 0;

    //! @copydoc mediaSetDefaults() const
    [[nodiscard]] virtual MediaSetDefaults& mediaSetDefaults() = 0;

    /**
     * @brief Sets the Media Set Defaults
     *
     * @param[in] mediaSetDefaults
     *   New Media Set Defaults
     **/
    virtual void mediaSetDefaults( MediaSetDefaults mediaSetDefaults ) = 0;

    /** @} **/

    /**
     * @name Configuration
     * @{
     **/

    /**
     * @brief Returns the configuration for the media set manager.
     *
     * @return Media Set Manager Configuration.
     **/
    [[nodiscard]] virtual MediaSetManagerConfiguration configuration() const = 0;

    /**
     * @brief Persist the Configuration.
     **/
    virtual void saveConfiguration() = 0;

    /**
     * @brief Returns the Media Set Manager Directory
     *
     * @return Media Set Manager Directory.
     **/
    [[nodiscard]] virtual const std::filesystem::path& directory() const = 0;

    /** @} **/

    /**
     * @name Media Set Management
     * @{
     **/

    /**
     * @brief Returns true if the Media Set with the given Part Number is registered to the Media Set Manager.
     *
     * @param[in] partNumber
     *   Media Set Part Number.
     *
     * @return If the media set with the given part number is registered
     **/
    [[nodiscard]] virtual bool hasMediaSet( std::string_view partNumber ) const = 0;

    /**
     * @brief Returns the Media Set with the given Part Number.
     *
     * @param[in] partNumber
     *   Media Set Part Number.
     *
     * @return Media Set with the given Part Number.
     * @retval {}
     *   If no Media Set with partNumber exist.
     **/
    [[nodiscard]] virtual std::optional< MediaSetInformation > mediaSet( std::string_view partNumber ) const = 0;

    /**
     * @brief Returns all registered media sets.
     *
     * @return All media sets.
     **/
    [[nodiscard]] virtual const MediaSetsInformation& mediaSets() const = 0;

    /**
     * @brief Registers the Media Set by the Media Set Manager.
     *
     * Reads the Media Set and imports them to the Media Set Manager.
     *
     * @param[in] mediaSetPaths
     *   Media Set Path Configuration
     * @param[in] checkFileIntegrity
     *   If set to true, additional file integrity steps are performed when loading the Media Set Manager.
     **/
    virtual void registerMediaSet( const MediaSetPaths &mediaSetPaths, bool checkFileIntegrity = true ) = 0;

    /**
     * @brief De-registers the Media Set from the Media Set Manager.
     *
     * The Media Set data is not deleted from the disk.
     *
     * @param[in] partNumber
     *   Part Number of Media Set
     *
     * @return Media Set Path Configuration (for removal)
     **/
    virtual MediaSetPaths deregisterMediaSet( std::string_view partNumber ) = 0;

    /**
     * @brief Returns the Check Values of the given Media Set.
     *
     * @param[in] mediaSet
     *   Media Set, where to get the Check Value information from.
     *
     * @return Check Values of @p mediaSet.
     **/
     virtual Media::CheckValues checkValues( Media::ConstMediaSetPtr mediaSet ) = 0;

    /** @} **/

    /**
     * @name Load Handling
     * @{
     **/

    /**
     * @brief Get all available Loads from all Media Sets.
     *
     * Iterates over every _ARINC 665 Media Set_ and retrieves all available loads.
     *
     * @return All available loads on all Media Sets.
     **/
    [[nodiscard]] virtual Media::ConstLoads loads() const = 0;

    /** @} **/

    /**
     * @name Batch Handling
     * @{
     **/

    /**
     * @brief Get all available Batches from all Media Sets.
     *
     * Iterates over every _ARINC 665 Media Set_ and retrieves all available batches.
     *
     * @return All available batches on all Media Sets.
     **/
    [[nodiscard]] virtual Media::ConstBatches batches() const = 0;

    /** @} **/

    /**
     * @name File Handling
     * @{
     **/

    /**
     * @brief Returns the path to the given file.
     *
     * Determins the filesystem path to the given file based on Media Set Manager and Media Set path information.
     *
     * @param[in] file
     *   File to request the path from
     *
     * @return Path to the given file.
     **/
    [[nodiscard]] virtual std::filesystem::path filePath( const Media::ConstFilePtr &file ) const = 0;

    /** @} **/
};

}

#endif
