// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief ARINC 615A Client Application (Command Line).
 **/

#include <arinc_615a_commands/Arinc615aCommands.hpp>

#include <arinc_665_commands/media_set_manager/MediaSetManager.hpp>

#include <commands/CommandRegistry.hpp>
#include <commands/Utils.hpp>

#include <spdlog/spdlog.h>

#include <boost/asio.hpp>

#include <boost/exception/all.hpp>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <thread>

//! I/O context
static boost::asio::io_context ioContext;
//! Signal Set
static boost::asio::signal_set signals{ ioContext, SIGINT, SIGTERM };

//! Abort Signal
static Arinc615aCommands::AbortTerminateSignal abortSignal;
//! Terminate Signal
static Arinc615aCommands::AbortTerminateSignal terminateSignal;

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

/**
 * @brief Signal Handler.
 *
 * Used to catch sigterm for graceful shutdown.
 * On sigterm @ref abortSignal is signalled.
 *
 * @param[in] error
 *   ASIO error code.
 * @param[in] signal
 *   Received signal.
 **/
static void signalHandler( const boost::system::error_code &error, int signal );

int main( int argc, char * argv[] )
{
  spdlog::set_level( spdlog::level::level_enum::warn );

  try
  {
    auto registry{ Commands::CommandRegistry::instance() };

    Arinc615aCommands::registerCommands( registry, ioContext, abortSignal, terminateSignal );

    Arinc665Commands::registerCommands( registry );

    signals.async_wait( std::bind_front(&signalHandler ) );

    auto ioRunner{
      std::jthread{ [](){
        ioContext.run();
      } } };

    const auto result{ Commands::Utils_commandLineHandler( registry )( argc, argv ) };

    signals.cancel();

    ioContext.stop();

    ioRunner.join();

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

static void signalHandler( const boost::system::error_code &error, const int signal )
{
  // handle abort
  if ( boost::asio::error::operation_aborted == error )
  {
    SPDLOG_INFO( "Signal handler aborted" );
    return;
  }

  switch ( signal )
  {
    case SIGINT:
      SPDLOG_INFO( "SIGINT received" );
      break;

    case SIGTERM:
      SPDLOG_INFO( "SIGTERM received" );
      break;

    default:
      SPDLOG_INFO( "Other signal received" );
      break;
  }

  // re-connect to SIGINT and SIGTERM
  signals.async_wait( std::bind_front( &signalHandler ) );

  // Abort indicator
  static bool abortIndicator{ false };

  // call the abort handler
  if ( !abortIndicator )
  {
    std::cout << "\nAbort request from user\n";
    abortSignal();
    abortIndicator = true;
  }
  else
  {
    std::cout << "\nTerminate request from user\n";
    terminateSignal();
  }
}
