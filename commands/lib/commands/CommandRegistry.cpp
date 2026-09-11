// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Commands::CommandRegistry.
 **/

#include "CommandRegistry.hpp"

#include "implementation/CommandRegistryImpl.hpp"

namespace Commands {

CommandRegistryPtr CommandRegistry::instance()
{
  return std::make_shared< CommandRegistryImpl >();
}

}
