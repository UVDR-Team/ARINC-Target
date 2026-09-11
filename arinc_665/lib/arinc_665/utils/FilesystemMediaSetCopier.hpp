// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemMediaSetCopier.
 **/

#ifndef ARINC_665_UTILS_FILESYSTEMMEDIASETCOPIER_HPP
#define ARINC_665_UTILS_FILESYSTEMMEDIASETCOPIER_HPP

#include <arinc_665/utils/Utils.hpp>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Copier.
 *
 * Copies a media set from a filesystem source to a filesystem destination.
 * No integrity checks of the media set are performed.
 **/
class ARINC_665_EXPORT FilesystemMediaSetCopier
{
  public:
    /**
     * @brief Creates the ARINC 665 %Media Set Copier Instance.
     *
     * @return ARINC 665 %Media Set Copier Instance
     **/
    [[nodiscard]] static FilesystemMediaSetCopierPtr create();

    //! Destructor
    virtual ~FilesystemMediaSetCopier() = default;

    /**
     * @name Configuration Methods.
     * @{
     **/

    /**
     * @brief Updates Source Media Paths.
     *
     * @param[in] mediaPaths
     *   Media Paths of Source Media Set.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCopier& mediaPaths( MediaPaths mediaPaths ) = 0;

    /**
     * @brief Updates the base-directory where the Media Set will be created.
     *
     * @param[in] outputBasePath
     *   Output Base directory.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCopier& outputBasePath( std::filesystem::path outputBasePath ) = 0;

    /**
     * @brief Updates the Media Set Name.
     *
     * The media set name is used for generating the output media set directory.
     * If the name is not set explicitly, the part number of the media set is used.
     *
     * @param[in] mediaSetName
     *   Media Set Name.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCopier& mediaSetName( std::string mediaSetName ) = 0;

    /** @} **/

    /**
     * @brief Executes the ARINC 665 %Media Set Copier.
     *
     * All parameters must have been set previously.
     *
     * @return Media Set Paths relative to Output Directory Base Path.
     *
     * @throw Arinc665Exception
     *   When the copy operation fails
     **/
    [[nodiscard]] virtual MediaSetPaths operator()() = 0;
};

}

#endif
