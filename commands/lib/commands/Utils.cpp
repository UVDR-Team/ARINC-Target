// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Commands Utils.
 **/

#include "Utils.hpp"

#include "implementation/CommandLineHandlerImpl.hpp"
#include "implementation/EchoCommandImpl.hpp"

#include <commands/CommandRegistry.hpp>

#include <format>
#include <functional>
#include <memory>

namespace Commands {

CommandLineHandler Utils_commandLineHandler( CommandRegistryPtr commandRegistry )
{
  auto commandLineHandler{ std::make_shared< CommandLineHandlerImpl >( std::move( commandRegistry ) ) };

  return std::bind_front( &CommandLineHandlerImpl::execute, commandLineHandler );
}

void Utils_echoCommand( CommandRegistry &commandRegistry )
{
  auto command{ std::make_shared< EchoCommandImpl>() };

  commandRegistry.command(
    "echo",
    "Echos the given data back",
    std::bind_front( &EchoCommandImpl::execute, command ),
    std::bind_front( &EchoCommandImpl::help, command ) );
}

void Utils_alias( CommandRegistry &commandRegistry, std::string_view aliasName, std::string_view commandName )
{
  const auto cmd{ commandRegistry.command( commandName ) };

  // we check execute function not to be null (comparison of functions is not allowed)
  if ( ( !cmd ) || std::get< 1 >( *cmd ) )
  {
    return;
  }

  commandRegistry.command(
    aliasName,
    std::format( "Alias for '{}'", std::string{ commandName } ),
    std::get< 1 >( *cmd ),
    std::get< 2 >( *cmd ) );
}

}
