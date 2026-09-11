// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::FilesystemMediaSetRemover.
 **/

#include "FilesystemMediaSetRemover.hpp"

#include <arinc_665/utils/implementation/FilesystemMediaSetRemoverImpl.hpp>

namespace Arinc665::Utils {

FilesystemMediaSetRemoverPtr FilesystemMediaSetRemover::create()
{
  return std::make_unique< FilesystemMediaSetRemoverImpl >();
}

}
