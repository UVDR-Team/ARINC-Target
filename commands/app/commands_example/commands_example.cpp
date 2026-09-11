// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Commands Example Application.
 **/

#include <commands/CommandRegistry.hpp>
#include <commands/Utils.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <spdlog/spdlog.h>

#include <cstddef>
#include <iostream>

/**
 * @brief Application Entry Point.
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   Arguments
 *
 * @return Application exit status.
 **/
int main( int argc, char * argv[] );

int main( const int argc, char * argv[] )
{
  spdlog::set_level( spdlog::level::warn );

  try
  {
    std::cout << "Commands Test\n";

    const auto registry{ Commands::CommandRegistry::instance() };

    Commands::Utils_echoCommand( *registry );
    Commands::Utils_alias( *registry, "echo2", "echo" );

    return Commands::Utils_commandLineHandler( registry )( argc, argv );
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Error: {}\n", boost::diagnostic_information( e ) );
    return EXIT_FAILURE;
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Error: {}\n", boost::diagnostic_information( e ) );
    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    std::cerr << "Unknown exception occurred\n";
    return EXIT_FAILURE;
  }
}
