// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Commands Utils.
 **/

#ifndef COMMANDS_UTILS_HPP
#define COMMANDS_UTILS_HPP

#include <commands/Commands.hpp>

#include <string_view>

namespace Commands {

/**
 * @name Command Handler Utilities.
 *
 * @{
 **/

/**
 * @brief Creates a Command Line Handler.
 *
 * @param[in] commandRegistry
 *   Command registry.
 *
 * @return Command line handler instance.
 **/
[[nodiscard]] COMMANDS_EXPORT CommandLineHandler Utils_commandLineHandler( CommandRegistryPtr commandRegistry );

/**
 * @brief Adds an echo command to the given command registry.
 *
 * @param[in] commandRegistry
 *   Command registry.
 **/
COMMANDS_EXPORT void Utils_echoCommand( CommandRegistry &commandRegistry );

/**
 * @brief Add an alias command.
 *
 * @param[in] commandRegistry
 *   Command registry.
 * @param[in] aliasName
 *   Name of new alias command.
 * @param[in] commandName
 *   Command Name of Aliased command.
 **/
COMMANDS_EXPORT void Utils_alias(
  CommandRegistry &commandRegistry,
  std::string_view aliasName,
  std::string_view commandName );

/** @} **/

}

#endif
