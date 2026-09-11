// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Commands::CommandRegistryImpl.
 **/

#ifndef COMMANDS_COMMANDREGISTRYIMPL_HPP
#define COMMANDS_COMMANDREGISTRYIMPL_HPP

#include <commands/CommandRegistry.hpp>

#include <map>
#include <string>

namespace Commands {

//! Implementation of CommandRegistry
class CommandRegistryImpl final : public CommandRegistry
{
  public:
    //! Default constructor
    CommandRegistryImpl() = default;

    //! Default destructor
    ~CommandRegistryImpl() noexcept override = default;

    //! @copydoc CommandRegistry::commands
    [[nodiscard]] const Commands& commands() const override;

    //! @copydoc CommandRegistry::command(std::string_view,std::string_view,ExecuteFunction,HelpFunction)
    void command(
      std::string_view name,
      std::string_view description,
      ExecuteFunction executeFunction,
      HelpFunction helpFunction ) override;

    //! @copydoc CommandRegistry::command(std::string_view)
    [[nodiscard]] std::optional< Command> command( std::string_view name ) override;

    //! @copydoc CommandRegistry::execute
    void execute( std::string_view name, const Parameters &parameters ) override;

    //! @copydoc CommandRegistry::help
    void help( std::string_view name ) override;

  private:
    //! Registered Commands
    Commands commandsV;
};

}

#endif
