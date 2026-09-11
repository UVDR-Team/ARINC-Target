// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Commands::CommandRegistry.
 **/

#ifndef COMMANDS_COMMANDREGISTRY_HPP
#define COMMANDS_COMMANDREGISTRY_HPP

#include <commands/Commands.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>

namespace Commands {

/**
 * @brief Command Registry.
 *
 * This class manages a collection of commands, where each command has a name,
 * a description, an execute handler, and a help handler.
 * It is responsible for managing command registration and execution.
 **/
class COMMANDS_EXPORT CommandRegistry {
  public:
    //! Command type (description, execute handler, help handler)
    using Command = std::tuple< std::string, ExecuteFunction, HelpFunction >;
    //! Commands type map of name -> command type
    using Commands = std::map< std::string, Command, std::less<> >;

    /**
     * @brief Creates a Command Registry Instance.
     *
     * @return Created Command Registry Instance.
     **/
    [[nodiscard]] static CommandRegistryPtr instance();

    //! Default destructor
    virtual ~CommandRegistry() noexcept = default;

    /**
     * @brief Returns the registered Commands.
     *
     * @return Registered Commands.
     **/
    [[nodiscard]] virtual const Commands& commands() const = 0;

    /**
     * @brief Registers a Command.
     *
     * @param[in] name
     *   Command name.
     * @param[in] description
     *   Command description.
     * @param[in] executeFunction
     *   Execute function handler-
     * @param[in] helpFunction
     *   Help function handler.
     **/
    virtual void command(
      std::string_view name,
      std::string_view description,
      ExecuteFunction executeFunction,
      HelpFunction helpFunction ) = 0;

    /**
     * @brief Return the Command associated with the given Name.
     *
     * @param[in] name
     *   Command Name
     *
     * @return Command for @p name.
     * @retval std::nullopt
     *   If command cannot be found.
     **/
    [[nodiscard]] virtual std::optional< Command > command( std::string_view name ) = 0;

    /**
     * @brief Executes the given Command with the given Parameters.
     *
     * @param[in] name
     *   Command Name
     * @param[in] parameters
     *   Command Parameters.
     *
     * @throw boost::program_options::invalid_option_value
     *   When @p name does not name a valid command.
     **/
    virtual void execute( std::string_view name, const Parameters &parameters ) = 0;

    /**
     * @brief Outputs a Command Help.
     *
     * Outputs help information for the command identified by @p name.
     *
     * @param[in] name
     *   Command Name
     *
     * @throw boost::program_options::invalid_option_value
     *   When @p name does not name a valid command.
     **/
    virtual void help( std::string_view name ) = 0;
};

}

#endif
