// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Commands::CommandLineHandlerImpl.
 **/

#include "CommandLineHandlerImpl.hpp"

#include <commands/CommandRegistry.hpp>

#include <boost/exception/all.hpp>

#include <cstddef>
#include <iostream>
#include <utility>

namespace Commands {

CommandLineHandlerImpl::CommandLineHandlerImpl( CommandRegistryPtr commandRegistry ) :
  commandRegistry{ std::move( commandRegistry ) }
{
  options.add_options()
  (
    "command,c",
    boost::program_options::value( &command )->value_name( "command" ),
    "Command to execute"
  )
  (
    "help,h",
    boost::program_options::bool_switch( &help )->default_value( false ),
    "Help"
  );
}

int CommandLineHandlerImpl::execute( const int argc, char const * const argv[] )
{
  boost::program_options::variables_map variablesMap;

  try
  {
    // parse command line
    const auto parsedOptions{
      boost::program_options::command_line_parser( argc, argv ).options( options ).allow_unregistered().run() };

    // extract unresolved options
    auto unparsedOptions{ boost::program_options::collect_unrecognized(
      parsedOptions.options,
      boost::program_options::exclude_positional ) };

    boost::program_options::store( parsedOptions, variablesMap );
    boost::program_options::notify( variablesMap );

    // check for command give
    if ( command.empty() )
    {
      if ( !help )
      {
        BOOST_THROW_EXCEPTION( boost::program_options::required_option{ "command" } );
      }

      // print help
      std::cout << std::format( "Usage of {}\n", argv[0] ) <<  options;

      std::cout << "Commands are:\n";

      for ( const auto &[ name, foundCommand ] : commandRegistry->commands() )
      {
        std::cout << std::format( " * {} - {}\n", name, std::get< 0>( foundCommand ) );
      }

      return EXIT_FAILURE;
    }

    if ( !help )
    {
      commandRegistry->execute( command, unparsedOptions );
    }
    else
    {
      commandRegistry->help( command );
    }
  }
  catch ( const boost::program_options::error &e )
  {
    std::cerr << std::format(
      "Error parsing command line: {}\n"
      "Enter {}{} --help for command line description.\n",
      e.what(),
      argv[ 0 ],
      command.empty() ? "" : " --command " + command );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

}
