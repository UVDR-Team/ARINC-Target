// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Commands::EchoCommandImpl.
 **/

#ifndef COMMANDS_ECHOCOMMANDIMPL_HPP
#define COMMANDS_ECHOCOMMANDIMPL_HPP

#include <commands/Commands.hpp>

#include <boost/program_options.hpp>

namespace Commands {

//! Implementation of Echo Command
class EchoCommandImpl final
{
  public:
    /**
     * @brief Initialises the Echo Command.
     **/
    EchoCommandImpl();

    /**
     * @brief Executes the Echo Command.
     *
     * @param[in] parameters
     *   Echo command line parameters.
     **/
    void execute( const Parameters &parameters );

    /**
     * @brief Prints the Help of the Echo Command.
     **/
    void help();

  private:
    //! Options Description
    boost::program_options::options_description options;

    //! Text to be Echoed.
    std::string echoText;
};

}
#endif
