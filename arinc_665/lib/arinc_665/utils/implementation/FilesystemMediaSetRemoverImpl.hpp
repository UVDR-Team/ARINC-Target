// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemMediaSetRemoverImpl.
 **/

#ifndef ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMMEDIASETREMOVERIMPL_HPP
#define ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMMEDIASETREMOVERIMPL_HPP

#include <arinc_665/utils/FilesystemMediaSetRemover.hpp>

namespace Arinc665::Utils {

//! Implementation of Filesystem %Media Set Remover.
class FilesystemMediaSetRemoverImpl final : public FilesystemMediaSetRemover
{
  public:
    //! Default Constructor
    FilesystemMediaSetRemoverImpl() = default;

    //! @copydoc FilesystemMediaSetRemover::mediaSetPaths()
    FilesystemMediaSetRemover& mediaSetPaths( MediaSetPaths mediaSetPaths ) override;

    void operator()() override;

  private:
    //! Media Set Paths
    MediaSetPaths mediaSetPathsV{};
};

}

#endif
