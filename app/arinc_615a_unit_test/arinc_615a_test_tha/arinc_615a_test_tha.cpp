// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @ingroup arinc_615a_test_tha
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Entry point of the ARINC 615A test THA target data loader.
 **/

#include "Arinc615aTha.hpp"
#include "TargetDataLoaderConfiguration.hpp"
#include "TargetInformationOperation.hpp"
#include "TargetMediaDefinedDownloadOperation.hpp"
#include "TargetOperatorDefinedDownloadOperation.hpp"
#include "TargetUploadOperation.hpp"

#include <arinc_615a/find/servers/Server.hpp>

#include <arinc_615a/target/ErrorOperation.hpp>
#include <arinc_615a/target/ErrorOperationConfiguration.hpp>
#include <arinc_615a/target/Protocol.hpp>
#include <arinc_615a/target/ProtocolConfiguration.hpp>




#include <tftp/packets/TftpOptions.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/exception/all.hpp>


#include <boost/asio/post.hpp>
#if ARINC_ENABLE_COMMAND_LINE
#include <boost/asio/signal_set.hpp>
#include <csignal>
#endif
#include <boost/asio/io_context.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

#include <cassert>
#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>

namespace {
class Runtime
{
public:
  explicit Runtime(const Arinc615aTha::TargetDataLoaderConfiguration &config) : configuration{ config } {}
  void run(bool hostSignals);
  void stop();
  void requestStop() { boost::asio::post(ioContext, [this] { stop(); }); }
private:
/**
 * @brief Signal Handler.
 *
 * Used to catch sigterm for graceful shutdown.
 *
 * @param[in] error
 *   ASIO error code.
 * @param[in] signal
 *   Received signal.
 **/
#if ARINC_ENABLE_COMMAND_LINE
void signalHandler( const boost::system::error_code &error, int signal );
#endif

/**
 * @brief Handles an ARINC 615A FIND Request.
 *
 * @param[in] from
 *   Source of request.
 **/
void findRequest( const boost::asio::ip::udp::endpoint &from );

/**
 * @brief Handles an ARINC 615A Operation Request.
 *
 * @param[in] remote
 *   Source of the request.
 * @param[in] operation
 *   ARINC 615A operation
 * @param[in] clientTftpOptions
 *   TFTP Client Options.
 * @param[in] port
 *   Received ARINC 615A Port Option.
 * @param[in] targetId
 *   Target ID
 **/
void operationRequest(
  Arinc615a::OperationType operation,
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Information Operation Request
 *
 * @param[in] remote
 *   Source of the request.
 * @param[in] opConfig
 *   Operation Configuration
 * @param[in] clientTftpOptions
 *   TFTP Client Options.
 * @param[in] port
 *   Received ARINC 615A Port Option.
 * @param[in] targetId
 *   Target ID
 **/
void informationOperationRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::InformationOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Upload Operation Request
 *
 * @param[in] remote
 *   Source of the request.
 * @param[in] opConfig
 *   Operation Configuration
 * @param[in] clientTftpOptions
 *   TFTP Client Options.
 * @param[in] port
 *   Received ARINC 615A Port Option.
 * @param[in] targetId
 *   Target ID
 **/
void uploadOperationRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::UploadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Media Defined Download Operation Request
 *
 * @param[in] remote
 *   Source of the request.
 * @param[in] opConfig
 *   Operation Configuration
 * @param[in] clientTftpOptions
 *   TFTP Client Options.
 * @param[in] port
 *   Received ARINC 615A Port Option.
 * @param[in] targetId
 *   Target ID
 **/
void mediaDefinedDownloadOperationRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Operator Defined Download Operation Request
 *
 * @param[in] remote
 *   Source of the request.
 * @param[in] opConfig
 *   Operation Configuration
 * @param[in] clientTftpOptions
 *   TFTP Client Options.
 * @param[in] port
 *   Received ARINC 615A Port Option.
 * @param[in] targetId
 *   Target ID
 **/
void operatorDefinedDownloadOperationRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Operation Finished handler.
 **/
void operationFinished();

/**
 * @brief Error Operation completed handler.
 **/
void errorOperationCompleted();

//! Target Data Loader Configuration
Arinc615aTha::TargetDataLoaderConfiguration configuration;

//! I/O Context
boost::asio::io_context ioContext;
//! Signal Set
#if ARINC_ENABLE_COMMAND_LINE
std::unique_ptr< boost::asio::signal_set > signals;
#endif
bool stopped{ false };

//! FIND Server
Arinc615a::Find::Servers::ServerPtr findServer;
//! ARINC 615A Target Protocol
Arinc615a::Target::ProtocolPtr protocol;
//! ARINC 615A Target Operation
std::shared_ptr< Arinc615aTha::TargetOperation > targetOperation;
//! Error Operation
Arinc615a::Target::ErrorOperationPtr errorOperation;

};

std::mutex activeMutex;
Runtime *activeRuntime{};

struct ActiveRuntimeGuard
{
  ~ActiveRuntimeGuard() { std::lock_guard lock{activeMutex}; activeRuntime = nullptr; }
};

int reportException() noexcept
{
  try { throw; }
  catch (const boost::exception &e) { std::cerr << "Error: " << boost::diagnostic_information(e) << '\n'; }
  catch (const std::exception &e) { std::cerr << "Error: " << e.what() << '\n'; }
  catch (...) { std::cerr << "Unknown exception occurred\n"; }
  return EXIT_FAILURE;
}

void Runtime::run(const bool hostSignals)
{
    findServer = Arinc615a::Find::Servers::Server::instance( ioContext );
    assert( findServer );

    findServer
      ->requestHandler( std::bind_front( &Runtime::findRequest, this ) )
      .localEndpoint( { configuration.find.localInterfaceAddress, configuration.find.findPort } );

    findServer->start();

    protocol = Arinc615a::Target::Protocol::instance(
      ioContext,
      Arinc615a::Target::ProtocolConfiguration{
        .newOperationRequestHandler = std::bind_front( &Runtime::operationRequest, this ),
        .configuration = configuration.dataLoader,
        .protocolVersion = configuration.version } );
    assert( protocol );

    protocol->start();

#if ARINC_ENABLE_COMMAND_LINE
  if (hostSignals)
  {
    signals = std::make_unique<boost::asio::signal_set>(ioContext, SIGINT, SIGTERM);
    signals->async_wait(std::bind_front(&Runtime::signalHandler, this));
  }
#else
  (void)hostSignals;
#endif
  // The caller's DKM task owns this event loop. No extra std::jthread.
  ioContext.run();
}

void Runtime::stop()
{
  if (stopped) return;
  stopped = true;
  std::cout << "Termination request\n";
  if (findServer) findServer->stop();
  if (protocol) protocol->stop();
#if ARINC_ENABLE_COMMAND_LINE
  if (signals) signals->cancel();
#endif
  ioContext.stop();
}

#if ARINC_ENABLE_COMMAND_LINE
void Runtime::signalHandler( const boost::system::error_code &error, const int signal )
{
  // handle abort
  if ( boost::asio::error::operation_aborted == error )
  {
    return;
  }

  switch ( signal )
  {
    case SIGINT:
     ARINC_LOG_INFO( "SIGINT received" );
      break;

    case SIGTERM:
     ARINC_LOG_INFO( "SIGTERM received" );
      break;

    default:
     ARINC_LOG_INFO( "Other signal received" );
      break;
  }

  stop();
}
#endif

void Runtime::findRequest( const boost::asio::ip::udp::endpoint &from )
{
  for ( const auto &findInformation : configuration.findInformation )
  {
    findServer->response( from, findInformation );
  }
}

void Runtime::operationRequest(
  const Arinc615a::OperationType operation,
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  ARINC_LOG_INFO( "Received operation from {} with Target ID: {}", remote.address().to_string(), targetId.toString() );

  auto targetInfo{ configuration.targets.find( static_cast< std::string >( targetId ) ) };

  if ( targetInfo == configuration.targets.end() )
  {
    ARINC_LOG_ERROR( "Invalid Target ID" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
        .operation = operation,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
        .description = "Invalid Target ID" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  // check active operation
  if ( targetOperation )
  {
   ARINC_LOG_ERROR( "Another operation already active" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
        .operation = operation,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
        .description = "Another operation already active" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  switch ( operation )
  {
    case Arinc615a::OperationType::Information:
      informationOperationRequest( remote, targetInfo->second.informationOperation, clientTftpOptions, port, targetId );
      break;

    case Arinc615a::OperationType::Upload:
      uploadOperationRequest( remote, targetInfo->second.uploadOperation, clientTftpOptions, port, targetId );
      break;

    case Arinc615a::OperationType::MediaDefinedDownload:
      mediaDefinedDownloadOperationRequest(
        remote,
        targetInfo->second.mediaDefinedDownloadOperation,
        clientTftpOptions,
        port,
        targetId );
      break;

    case Arinc615a::OperationType::OperatorDefinedDownload:
      operatorDefinedDownloadOperationRequest(
        remote,
        targetInfo->second.operatorDefinedDownloadOperation,
        clientTftpOptions,
        port,
        targetId );
      break;

    default:
     ARINC_LOG_ERROR( "Invalid Operation Request" );

      errorOperation =
        protocol->errorOperation( Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
          .operation = operation,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
          .description = "Invalid Operation Request" } );

      errorOperation->start( remote, clientTftpOptions, port );
      return;
  }
}

void Runtime::informationOperationRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::InformationOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  // If operation is not enabled, reject it
  if ( !opConfig.enabled )
  {
   ARINC_LOG_ERROR( "Information Operation not Enabled" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
        .operation = Arinc615a::OperationType::Information,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  targetOperation = std::make_shared< Arinc615aTha::TargetInformationOperation >(
    std::bind_front( &Runtime::operationFinished, this ),
    ioContext,
    opConfig,
    *protocol,
    targetId,
    configuration.statusTransmissionRate );

  targetOperation->initialise( remote, clientTftpOptions, port );
}

void Runtime::uploadOperationRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::UploadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  // If operation is not enabled, reject it
  if ( !opConfig.enabled )
  {
   ARINC_LOG_ERROR( "Upload Operation not Enabled" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
        .operation = Arinc615a::OperationType::Upload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  if ( !std::filesystem::is_directory( opConfig.directory ) )
  {
   ARINC_LOG_ERROR( "Upload Directory does not exist" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
        .operation = Arinc615a::OperationType::Upload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
        .description = "Upload Directory does not exist" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  targetOperation = std::make_shared< Arinc615aTha::TargetUploadOperation >(
    std::bind_front( &Runtime::operationFinished, this ),
    ioContext,
    opConfig,
    *protocol,
    targetId,
    configuration.statusTransmissionRate );

  targetOperation->initialise( remote, clientTftpOptions, port );
}

void Runtime::mediaDefinedDownloadOperationRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  // If operation is not enabled, reject it
  if ( !opConfig.enabled )
  {
   ARINC_LOG_ERROR( "Operation not enabled" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
        .operation = Arinc615a::OperationType::MediaDefinedDownload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  for ( const auto &directory : opConfig.directories )
  {
    if ( !std::filesystem::is_directory( directory ) )
    {
     ARINC_LOG_ERROR( "Download Directory does not exist: '{}'", directory.string() );

      errorOperation = protocol->errorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
          .operation = Arinc615a::OperationType::MediaDefinedDownload,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
          .description = "Download Directory does not exist" } );

      errorOperation->start( remote, clientTftpOptions, port );

      return;
    }
  }

  targetOperation = std::make_shared< Arinc615aTha::TargetMediaDefinedDownloadOperation >(
    std::bind_front( &Runtime::operationFinished, this ),
    ioContext,
    opConfig,
    *protocol,
    targetId,
    configuration.statusTransmissionRate );

  targetOperation->initialise( remote, clientTftpOptions, port );
}

void Runtime::operatorDefinedDownloadOperationRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  // If operation is not enabled, reject it
  if ( !opConfig.enabled )
  {
   ARINC_LOG_ERROR( "Operation not Enabled" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
        .operation = Arinc615a::OperationType::OperatorDefinedDownload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  for ( const auto &directory : opConfig.directories )
  {
    if ( !std::filesystem::is_directory( directory ) )
    {
     ARINC_LOG_ERROR( "Download Directory does not exist: '{}'", directory.string() );

      errorOperation = protocol->errorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = std::bind_front( &Runtime::errorOperationCompleted, this ),
          .operation = Arinc615a::OperationType::OperatorDefinedDownload,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
          .description = "Download Directory does not exist" } );

      errorOperation->start( remote, clientTftpOptions, port );

      return;
    }
  }

  targetOperation = std::make_shared< Arinc615aTha::TargetOperatorDefinedDownloadOperation >(
    std::bind_front( &Runtime::operationFinished, this ),
    ioContext,
    opConfig,
    *protocol,
    targetId,
    configuration.statusTransmissionRate );

  targetOperation->initialise( remote, clientTftpOptions, port );
}

void Runtime::operationFinished()
{
  ARINC_LOG_INFO( "Operation finished" );

  targetOperation.reset();
}

void Runtime::errorOperationCompleted()
{
  ARINC_LOG_INFO( "Error Operation finished" );

  errorOperation.reset();
}

} // namespace

int Arinc615aTha::detail::runConfigured(const TargetDataLoaderConfiguration &config, const bool hostSignals)
{
  try
  {
    Runtime runtime{config};
    {
      std::lock_guard lock{activeMutex};
      if (activeRuntime)
      {
        std::cerr << "ARINC 615A THA is already running\n";
        return EXIT_FAILURE;
      }
      activeRuntime = &runtime;
    }
    ActiveRuntimeGuard guard;
    runtime.run(hostSignals);
    return EXIT_SUCCESS;
  }
  catch (...) { return reportException(); }
}

int Arinc615aTha::run(const TargetDataLoaderConfiguration &config)
{
  std::cout << "ARINC 615A THA\n";
  return detail::runConfigured(config, false);
}

int Arinc615aTha::runFromFile(const char *configurationFile)
{
  try
  {
    if (!configurationFile || !*configurationFile)
      throw std::invalid_argument{"Configuration filename is empty"};
    std::cout << "ARINC 615A THA\n";
    boost::property_tree::ptree properties;
    if (std::filesystem::is_regular_file(configurationFile))
      boost::property_tree::read_json(configurationFile, properties);
    else
      throw std::invalid_argument{std::string{"Configuration file does not exist: "} + configurationFile};
    return detail::runConfigured(TargetDataLoaderConfiguration{properties}, false);
  }
  catch (...) { return reportException(); }
}

bool Arinc615aTha::requestStop() noexcept
{
  try
  {
    std::lock_guard lock{activeMutex};
    if (!activeRuntime) return false;
    activeRuntime->requestStop();
    return true;
  }
  catch (...) { return false; }
}
