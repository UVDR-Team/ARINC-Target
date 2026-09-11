// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc665Commands.
 **/

#include "Arinc665Commands.hpp"

#include <arinc_665_commands/media_set_manager/MediaSetManager.hpp>

namespace Arinc665Commands {

void registerCommands( const Commands::CommandRegistryPtr& registry )
{
  MediaSetManager::registerCommands( registry );
}

void setLogLevel( const spdlog::level::level_enum level )
{
  spdlog::set_level( level );
}

}
