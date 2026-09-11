// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief ARINC 665 Media Set Manager.
 **/

#include <arinc_665_commands/Arinc665Commands.hpp>

#include <arinc_665/Version.hpp>

#include <arinc_649/Arinc649.hpp>

#include <commands/CommandRegistry.hpp>
#include <commands/Utils.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/diagnostic_information.hpp>

#include <cstdlib>
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
try
{
  spdlog::set_level( spdlog::level::level_enum::warn );
  Arinc665Commands::setLogLevel( spdlog::level::level_enum::warn );
  Arinc665::setLogLevel( spdlog::level::level_enum::warn );
  Arinc649::setLogLevel( spdlog::level::level_enum::warn );
  Commands::setLogLevel( spdlog::level::level_enum::warn );
  Helper::setLogLevel( spdlog::level::level_enum::warn );

  try
  {
    std::cout << std::format( "ARINC 665 Media Set Manager - {}\n", Arinc665::Version::VersionInformation );

    const auto registry{ Commands::CommandRegistry::instance() };

    Arinc665Commands::registerCommands( registry );

    const auto result{ Commands::Utils_commandLineHandler( registry )( argc, argv ) };

    return result;
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
catch ( ... )
{
  std::cerr << "Very bad exception\n";
  return EXIT_FAILURE;
}
