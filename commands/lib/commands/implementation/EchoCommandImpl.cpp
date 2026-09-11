// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Commands::EchoCommandImpl.
 **/

#include "EchoCommandImpl.hpp"

#include <iostream>

namespace Commands {

EchoCommandImpl::EchoCommandImpl() :
  options{ "Echo options" }
{
  options.add_options()
  (
    "text", boost::program_options::value( &echoText )->required(), "Echoed text"
  );
}

void EchoCommandImpl::execute( const Parameters &parameters)
{
  boost::program_options::variables_map variablesMap;
  boost::program_options::store(
    boost::program_options::command_line_parser( parameters ).options( options ).run(),
    variablesMap );
  boost::program_options::notify( variablesMap );

  std::cout << "Echo: '" << echoText << "'\n";
}

void EchoCommandImpl::help()
{
  std::cout << "Echos the supplied text\n";

  std::cout << options;
}

}
