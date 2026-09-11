// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Commands::CommandLineHandlerImpl.
 **/

#ifndef COMMANDS_COMMANDLINEHANDLERIMPL_HPP
#define COMMANDS_COMMANDLINEHANDLERIMPL_HPP

#include <commands/Commands.hpp>

#include <boost/program_options.hpp>

namespace Commands {

//! Command Line Handler.
class CommandLineHandlerImpl final
{
  public:
    /**
     * @brief Initialises the Command Line Handler.
     *
     * @param[in] commandRegistry
     *   The command registry used.
     **/
    explicit CommandLineHandlerImpl( CommandRegistryPtr commandRegistry );

    /**
     * @brief Executes the command line handler
     *
     * @param[in] argc
     *   Argument count.
     * @param[in] argv
     *   Argument values.
     *
     * @return The exit code
     * @retval EXIT_SUCCESS
     * @retval EXIT_FAILURE
     **/
    [[nodiscard]] int execute( int argc, char const * const argv[] );

  private:
    //! Command registry
    CommandRegistryPtr commandRegistry;
    //! Used options description
    boost::program_options::options_description options{ "Command Options" };
    //! Command string
    std::string command;
    //! help option
    bool help{ false };
};

}

#endif
