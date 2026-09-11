// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc665Commands::MediaSetManager.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc665Commands::MediaSetManager.
 **/

#ifndef ARINC_665_COMMANDS_MEDIA_SET_MANAGER_MEDIASETMANAGER_HPP
#define ARINC_665_COMMANDS_MEDIA_SET_MANAGER_MEDIASETMANAGER_HPP

#include <arinc_665_commands/Arinc665Commands.hpp>

/**
 * @brief ARINC 665 %Media Set Manager %Commands.
 *
 * %Commands, which provides ARINC 665 Media Set Manager functionality by using the @ref Commands library.
 **/
namespace Arinc665Commands::MediaSetManager {

/**
 * @brief Registers the ARINC 665 %Media Set Manager %Commands to the Registry.
 *
 * @param[in,out] registry
 *   Registry, where the commands shall be registered to.
 **/
ARINC_665_COMMANDS_EXPORT void registerCommands( const Commands::CommandRegistryPtr& registry );

}

#endif
