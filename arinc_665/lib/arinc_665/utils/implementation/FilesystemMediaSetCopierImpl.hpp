// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemMediaSetCopierImpl.
 **/

#ifndef ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMMEDIASETCOPIERIMPL_HPP
#define ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMMEDIASETCOPIERIMPL_HPP

#include <arinc_665/utils/FilesystemMediaSetCopier.hpp>

namespace Arinc665::Utils {

//! Implementation of Filesystem %Media Set Copier.
class FilesystemMediaSetCopierImpl final : public FilesystemMediaSetCopier
{
  public:
    //! Default Constructor
    FilesystemMediaSetCopierImpl() = default;

    //! @copydoc FilesystemMediaSetCopier::mediaPaths()
    FilesystemMediaSetCopier& mediaPaths( MediaPaths mediaPaths ) override;

    //! @copydoc FilesystemMediaSetCopier::outputBasePath()
    FilesystemMediaSetCopier& outputBasePath( std::filesystem::path outputBasePath ) override;

    //! @copydoc FilesystemMediaSetCopier::mediaSetName()
    FilesystemMediaSetCopier& mediaSetName( std::string mediaSetName ) override;

    [[nodiscard]] MediaSetPaths operator()() override;

  private:
    //! Media Paths
    MediaPaths mediaPathsV;
    //! Output Base Path
    std::filesystem::path outputBasePathV;
    //! Media Set Name
    std::string mediaSetNameV;
};

}

#endif
