// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::MediaSetManagerImpl.
 **/

#ifndef ARINC_665_UTILS_MEDIASETMANAGERIMPL_HPP
#define ARINC_665_UTILS_MEDIASETMANAGERIMPL_HPP

#include <arinc_665/utils/Utils.hpp>
#include <arinc_665/utils/MediaSetManager.hpp>
#include <arinc_665/utils/MediaSetManagerConfiguration.hpp>

#include <arinc_665/files/Files.hpp>

#include <arinc_649/CheckValue.hpp>

namespace Arinc665::Utils {

/**
 * @brief Implementation of MediaSetManager.
 *
 * Uses the FilesystemMediaSetManager to import the Media Sets from the disk.
 **/
class MediaSetManagerImpl final : public MediaSetManager
{
  public:
    /**
     * @brief Initialises the media set manager.
     *
     * @param[in] directory
     *   Directory for Media Set Manger.
     * @param[in] checkFileIntegrity
     *   If set to @p true, additional file integrity steps are performed.
     * @param[in] loadProgressHandler
     *   Handler for load progress.
     **/
    MediaSetManagerImpl(
      std::filesystem::path directory,
      bool checkFileIntegrity,
      const LoadProgressHandler &loadProgressHandler );

    ~MediaSetManagerImpl() override;

    //! @copydoc MediaSetManager::mediaSetDefaults() const
    [[nodiscard]] const MediaSetDefaults&
    mediaSetDefaults() const override;

    //! @copydoc MediaSetManager::mediaSetDefaults()
    [[nodiscard]] MediaSetDefaults& mediaSetDefaults() override;

    //! @copydoc MediaSetManager::mediaSetDefaults(MediaSetDefaults)
    void mediaSetDefaults( MediaSetDefaults mediaSetDefaults ) override;

    //! @copydoc MediaSetManager::configuration
    [[nodiscard]] MediaSetManagerConfiguration configuration() const override;

    //! @copydoc MediaSetManager::saveConfiguration()
    void saveConfiguration() override;

    //! @copydoc MediaSetManager::directory()
    [[nodiscard]] const std::filesystem::path& directory() const override;

    //! @copydoc MediaSetManager::hasMediaSet(std::string_view) const
    [[nodiscard]] bool hasMediaSet( std::string_view partNumber ) const override;

    //! @copydoc MediaSetManager::mediaSet(std::string_view) const
    [[nodiscard]] std::optional< MediaSetInformation > mediaSet( std::string_view partNumber ) const override;

    //! @copydoc MediaSetManager::mediaSets() const
    [[nodiscard]] const MediaSetsInformation& mediaSets() const override;

    //! @copydoc MediaSetManager::registerMediaSet()
    void registerMediaSet( const MediaSetPaths &mediaSetPaths, bool checkFileIntegrity = true ) override;

    //! @copydoc MediaSetManager::deregisterMediaSet()
    [[nodiscard]] MediaSetPaths deregisterMediaSet( std::string_view partNumber ) override;

    //! @copydoc MediaSetManager::checkValues()
    Media::CheckValues checkValues( Media::ConstMediaSetPtr mediaSet ) override;

    //! @copydoc MediaSetManager::loads() const
    [[nodiscard]] Media::ConstLoads loads() const override;

    //! @copydoc MediaSetManager::batches() const
    [[nodiscard]] Media::ConstBatches batches() const override;

    //! @copydoc MediaSetManager::filePath
    [[nodiscard]] std::filesystem::path filePath( const Media::ConstFilePtr &file ) const override;

  private:
    /**
     * @brief Load Media Sets.
     *
     * @param[in] mediaSetsPaths
     *   Media Sets Paths.
     * @param[in] checkFileIntegrity
     *   If set to @p true, additional file integrity steps are performed.
     * @param[in] loadProgressHandler
     *   Handler for load progress.
     **/
    void loadMediaSets(
      const MediaSetManagerConfiguration::MediaSetsPaths &mediaSetsPaths,
      bool checkFileIntegrity,
      const LoadProgressHandler &loadProgressHandler );

    /**
     * @brief Converts the given Media Set Paths to absolute Media Paths.
     *
     * Uses the base path, media set path, and media paths to convert it to absolute paths.
     *
     * @param[in] mediaSetPaths
     *   Media Set Paths information.
     *
     * @return Absolute Media Paths.
     **/
    [[nodiscard]] MediaPaths absoluteMediaPaths( const MediaSetPaths &mediaSetPaths ) const;

    //! Media Set Paths Map (Part Number -> Media Set Paths)
    using MediaSetsPaths = std::map< std::string, MediaSetPaths, std::less<> >;

    //! Media Set Manager Directory
    const std::filesystem::path directoryV;
    //! Media Set Defaults
    MediaSetDefaults mediaSetDefaultsV;
    //! Media Sets Information
    MediaSetsInformation mediaSetsInformationV;
    //! Media Sets Paths
    MediaSetsPaths mediaSetsPathsV;
};

}

#endif
