// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc665Commands::MediaSetManager.
 **/

#include "MediaSetManager.hpp"

#include <arinc_665_commands/media_set_manager/CreateMediaSetManagerCommand.hpp>
#include <arinc_665_commands/media_set_manager/ListBatchesCommand.hpp>
#include <arinc_665_commands/media_set_manager/ListLoadsCommand.hpp>
#include <arinc_665_commands/media_set_manager/ListMediaSetsCommand.hpp>
#include <arinc_665_commands/media_set_manager/ImportMediaSetXmlCommand.hpp>
#include <arinc_665_commands/media_set_manager/ImportMediaSetCommand.hpp>
#include <arinc_665_commands/media_set_manager/RemoveMediaSetCommand.hpp>

#include <commands/CommandRegistry.hpp>

#include <functional>
#include <memory>

namespace Arinc665Commands::MediaSetManager {

void registerCommands( const Commands::CommandRegistryPtr& registry )
{
  auto createMediaSetManagerCommand{ std::make_shared< CreateMediaSetManagerCommand >() };
  registry->command(
    "Create",
    "Create ARINC 665 Media Set Manager",
    std::bind_front( &CreateMediaSetManagerCommand::execute, createMediaSetManagerCommand ),
    std::bind_front( &CreateMediaSetManagerCommand::help, createMediaSetManagerCommand ) );

  auto listLoadsCommand{ std::make_shared< ListLoadsCommand >() };
  registry->command(
    "ListLoads",
    "List Loads contained within all ARINC 665 Media Sets",
    std::bind_front( &ListLoadsCommand::execute, listLoadsCommand ),
    std::bind_front( &ListLoadsCommand::help, listLoadsCommand ) );


  auto listBatchesCommand{ std::make_shared< ListBatchesCommand >() };
  registry->command(
    "ListBatches",
    "List batches contained within all ARINC 665 Media Sets",
    std::bind_front( &ListBatchesCommand::execute, listBatchesCommand ),
    std::bind_front( &ListBatchesCommand::help, listBatchesCommand ) );

  auto listCommand{ std::make_shared< ListMediaSetsCommand >() };
  registry->command(
    "ListMediaSets",
    "List ARINC 665 Media Sets",
    std::bind_front( &ListMediaSetsCommand::execute, listCommand ),
    std::bind_front( &ListMediaSetsCommand::help, listCommand ) );

  auto importXmlCommand{ std::make_shared< ImportMediaSetXmlCommand >() };
  registry->command(
    "ImportMediaSetXml",
    "Import ARINC Media Set XML",
    std::bind_front( &ImportMediaSetXmlCommand::execute, importXmlCommand ),
    std::bind_front( &ImportMediaSetXmlCommand::help, importXmlCommand ) );

  auto importCommand{ std::make_shared< ImportMediaSetCommand >() };
  registry->command(
    "ImportMediaSet",
    "Import ARINC 665 Media Set",
    std::bind_front( &ImportMediaSetCommand::execute, importCommand ),
    std::bind_front( &ImportMediaSetCommand::help, importCommand ) );

  auto removeMediaSetCommand{ std::make_shared< RemoveMediaSetCommand >() };
  registry->command(
    "RemoveMediaSet",
    "Remove ARINC 665 Media Set",
    std::bind_front( &RemoveMediaSetCommand::execute, removeMediaSetCommand ),
    std::bind_front( &RemoveMediaSetCommand::help, removeMediaSetCommand ) );
}

}
