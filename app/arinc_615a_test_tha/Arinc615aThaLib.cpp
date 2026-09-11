// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @ingroup arinc_615a_test_tha
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 *
 * @brief Implementation of ARINC 615A THA Library and C DKM interface.
 **/

#include "Arinc615aThaLib.hpp"
#include "arinc_615a_tha.h"

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

#include <arinc_615a/information/Information.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#ifdef m_data
#undef m_data
#endif
#ifdef m_type
#undef m_type
#endif
#ifdef m_flags
#undef m_flags
#endif

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <atomic>
#include <memory>
#include <thread>
#include <sstream>
#include <fstream>
#include <iostream>

namespace {

//! Active configuration
Arinc615aTha::TargetDataLoaderConfiguration g_configuration;

//! I/O context
std::unique_ptr< boost::asio::io_context > g_ioContext;

//! Running flag
std::atomic< bool > g_running{ false };

//! Background worker thread (when runInBackground = true)
std::unique_ptr< std::thread > g_workerThread;

//! FIND Server
Arinc615a::Find::Servers::ServerPtr g_findServer;

//! ARINC 615A Target Protocol
Arinc615a::Target::ProtocolPtr g_protocol;

//! ARINC 615A Target Operation
std::shared_ptr< Arinc615aTha::TargetOperation > g_targetOperation;

//! Error Operation
Arinc615a::Target::ErrorOperationPtr g_errorOperation;

// Forward declarations of internal protocol handlers
void findRequest(
  const Arinc615a::Find::TargetsInformation &targetsInformation,
  const boost::asio::ip::udp::endpoint &from );

void operationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  Arinc615a::OperationType operation,
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

void informationOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::InformationOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

void uploadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::UploadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

void mediaDefinedDownloadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

void operatorDefinedDownloadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

void operationFinished();
void errorOperationCompleted();

void findRequest(
  const Arinc615a::Find::TargetsInformation &targetsInformation,
  const boost::asio::ip::udp::endpoint &from )
{
  if ( !g_findServer )
  {
    return;
  }
  for ( const auto &findInformation : targetsInformation )
  {
    g_findServer->response( from, findInformation );
  }
}

void operationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const Arinc615a::OperationType operation,
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  SPDLOG_INFO(
    "Received operation from {} with Target ID: {}",
    remote.address().to_string(),
    targetId.toString() );

  const auto targetInfo{ configuration.targets.find( static_cast< std::string >( targetId ) ) };

  if ( targetInfo == configuration.targets.end() )
  {
    SPDLOG_ERROR( "Invalid Target ID" );

    g_errorOperation = g_protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = &errorOperationCompleted,
        .operation = operation,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
        .description = "Invalid Target ID" } );

    g_errorOperation->start( remote, clientTftpOptions, port );
    return;
  }

  // check active operation
  if ( g_targetOperation )
  {
    SPDLOG_ERROR( "Another operation already active" );

    g_errorOperation = g_protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = &errorOperationCompleted,
        .operation = operation,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
        .description = "Another operation already active" } );

    g_errorOperation->start( remote, clientTftpOptions, port );
    return;
  }

  switch ( operation )
  {
    case Arinc615a::OperationType::Information:
      informationOperationRequest(
        ioContext,
        configuration,
        remote,
        targetInfo->second.informationOperation,
        clientTftpOptions,
        port,
        targetId );
      break;

    case Arinc615a::OperationType::Upload:
      uploadOperationRequest(
        ioContext,
        configuration,
        remote,
        targetInfo->second.uploadOperation,
        clientTftpOptions,
        port,
        targetId );
      break;

    case Arinc615a::OperationType::MediaDefinedDownload:
      mediaDefinedDownloadOperationRequest(
        ioContext,
        configuration,
        remote,
        targetInfo->second.mediaDefinedDownloadOperation,
        clientTftpOptions,
        port,
        targetId );
      break;

    case Arinc615a::OperationType::OperatorDefinedDownload:
      operatorDefinedDownloadOperationRequest(
        ioContext,
        configuration,
        remote,
        targetInfo->second.operatorDefinedDownloadOperation,
        clientTftpOptions,
        port,
        targetId );
      break;

    default:
      SPDLOG_ERROR( "Invalid Operation Request" );

      g_errorOperation = g_protocol->errorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = &errorOperationCompleted,
          .operation = operation,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
          .description = "Invalid Operation Request" } );

      g_errorOperation->start( remote, clientTftpOptions, port );
      return;
  }
}

void informationOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::InformationOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  if ( !opConfig.enabled )
  {
    SPDLOG_ERROR( "Information Operation not Enabled" );

    g_errorOperation = g_protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = &errorOperationCompleted,
        .operation = Arinc615a::OperationType::Information,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    g_errorOperation->start( remote, clientTftpOptions, port );
    return;
  }

  g_targetOperation = std::make_shared< Arinc615aTha::TargetInformationOperation >(
    &operationFinished,
    ioContext,
    opConfig,
    *g_protocol,
    targetId,
    configuration.statusTransmissionRate );

  g_targetOperation->initialise( remote, clientTftpOptions, port );
}

void uploadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::UploadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  if ( !opConfig.enabled )
  {
    SPDLOG_ERROR( "Upload Operation not Enabled" );

    g_errorOperation = g_protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = &errorOperationCompleted,
        .operation = Arinc615a::OperationType::Upload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    g_errorOperation->start( remote, clientTftpOptions, port );
    return;
  }

  if ( std::filesystem::is_directory( opConfig.directory ) )
  {
    SPDLOG_ERROR( "Upload Directory does not exist" );

    g_errorOperation = g_protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = &errorOperationCompleted,
        .operation = Arinc615a::OperationType::Upload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
        .description = "Upload Directory does not exist" } );

    g_errorOperation->start( remote, clientTftpOptions, port );
    return;
  }

  g_targetOperation = std::make_shared< Arinc615aTha::TargetUploadOperation >(
    &operationFinished,
    ioContext,
    opConfig,
    *g_protocol,
    targetId,
    configuration.statusTransmissionRate );

  g_targetOperation->initialise( remote, clientTftpOptions, port );
}

void mediaDefinedDownloadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  if ( !opConfig.enabled )
  {
    SPDLOG_ERROR( "Operation not enabled" );

    g_errorOperation = g_protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = &errorOperationCompleted,
        .operation = Arinc615a::OperationType::MediaDefinedDownload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    g_errorOperation->start( remote, clientTftpOptions, port );
    return;
  }

  for ( const auto &directory : opConfig.directories )
  {
    if ( !std::filesystem::is_directory( directory ) )
    {
      SPDLOG_ERROR( "Download Directory does not exist: '{}'", directory.string() );

      g_errorOperation = g_protocol->errorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = &errorOperationCompleted,
          .operation = Arinc615a::OperationType::MediaDefinedDownload,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
          .description = "Download Directory does not exist" } );

      g_errorOperation->start( remote, clientTftpOptions, port );
      return;
    }
  }

  g_targetOperation = std::make_shared< Arinc615aTha::TargetMediaDefinedDownloadOperation >(
    &operationFinished,
    ioContext,
    opConfig,
    *g_protocol,
    targetId,
    configuration.statusTransmissionRate );

  g_targetOperation->initialise( remote, clientTftpOptions, port );
}

void operatorDefinedDownloadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  if ( !opConfig.enabled )
  {
    SPDLOG_ERROR( "Operation not Enabled" );

    g_errorOperation = g_protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = &errorOperationCompleted,
        .operation = Arinc615a::OperationType::OperatorDefinedDownload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    g_errorOperation->start( remote, clientTftpOptions, port );
    return;
  }

  for ( const auto &directory : opConfig.directories )
  {
    if ( !std::filesystem::is_directory( directory ) )
    {
      SPDLOG_ERROR( "Download Directory does not exist: '{}'", directory.string() );

      g_errorOperation = g_protocol->errorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = &errorOperationCompleted,
          .operation = Arinc615a::OperationType::OperatorDefinedDownload,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
          .description = "Download Directory does not exist" } );

      g_errorOperation->start( remote, clientTftpOptions, port );
      return;
    }
  }

  g_targetOperation = std::make_shared< Arinc615aTha::TargetOperatorDefinedDownloadOperation >(
    &operationFinished,
    ioContext,
    opConfig,
    *g_protocol,
    targetId,
    configuration.statusTransmissionRate );

  g_targetOperation->initialise( remote, clientTftpOptions, port );
}

void operationFinished()
{
  SPDLOG_INFO( "Operation finished" );
  g_targetOperation.reset();
}

void errorOperationCompleted()
{
  SPDLOG_INFO( "Error Operation finished" );
  g_errorOperation.reset();
}

} // namespace

namespace Arinc615aTha {

int init( const boost::property_tree::ptree &properties )
{
  try
  {
    g_configuration.fromProperties( properties );
    return ARINC615A_OK;
  }
  catch ( const std::exception &e )
  {
    SPDLOG_ERROR( "Exception during THA init: {}", e.what() );
    return ARINC615A_ERROR;
  }
  catch ( ... )
  {
    SPDLOG_ERROR( "Unknown exception during THA init" );
    return ARINC615A_ERROR;
  }
}

int initFromJson( std::string_view jsonStr )
{
  try
  {
    std::string s( jsonStr );
    std::istringstream stream( s );
    boost::property_tree::ptree pt;
    boost::property_tree::read_json( stream, pt );
    return init( pt );
  }
  catch ( const std::exception &e )
  {
    SPDLOG_ERROR( "Error parsing JSON configuration: {}", e.what() );
    return ARINC615A_ERROR;
  }
}

int initFromFile( const std::filesystem::path &jsonFilePath )
{
  try
  {
    if ( !std::filesystem::is_regular_file( jsonFilePath ) )
    {
      SPDLOG_ERROR( "Configuration file not found: {}", jsonFilePath.string() );
      return ARINC615A_ERROR;
    }
    boost::property_tree::ptree pt;
    boost::property_tree::read_json( jsonFilePath.string(), pt );
    return init( pt );
  }
  catch ( const std::exception &e )
  {
    SPDLOG_ERROR( "Error reading JSON configuration file {}: {}", jsonFilePath.string(), e.what() );
    return ARINC615A_ERROR;
  }
}

int initDefault()
{
  g_configuration = TargetDataLoaderConfiguration{};
  return ARINC615A_OK;
}

int start( bool runInBackground )
{
  if ( g_running.load() )
  {
    SPDLOG_WARN( "ARINC 615A THA Target is already running." );
    return ARINC615A_ALREADY_RUN;
  }

  try
  {
    g_ioContext = std::make_unique< boost::asio::io_context >();

    g_findServer = Arinc615a::Find::Servers::Server::instance( *g_ioContext );
    if ( !g_findServer )
    {
      SPDLOG_ERROR( "Failed to create FIND Server instance" );
      return ARINC615A_ERROR;
    }

    g_findServer
      ->requestHandler( std::bind_front( &findRequest, std::cref( g_configuration.findInformation ) ) )
      .localEndpoint( { g_configuration.find.localInterfaceAddress, g_configuration.find.findPort } );
    g_findServer->start();

    g_protocol = Arinc615a::Target::Protocol::instance(
      *g_ioContext,
      Arinc615a::Target::ProtocolConfiguration{
        .newOperationRequestHandler =
          std::bind_front( &operationRequest, std::ref( *g_ioContext ), std::cref( g_configuration ) ),
        .configuration = g_configuration.dataLoader,
        .protocolVersion = g_configuration.version } );

    if ( !g_protocol )
    {
      SPDLOG_ERROR( "Failed to create Protocol instance" );
      g_findServer->stop();
      g_findServer.reset();
      return ARINC615A_ERROR;
    }

    g_protocol->start();
    g_running.store( true );

    if ( runInBackground )
    {
      g_workerThread = std::make_unique< std::thread >( []{
        try
        {
          g_ioContext->run();
        }
        catch ( const std::exception &e )
        {
          SPDLOG_ERROR( "Exception in ioContext runner: {}", e.what() );
        }
        catch ( ... )
        {
          SPDLOG_ERROR( "Unknown exception in ioContext runner" );
        }
        g_running.store( false );
      } );
    }
    else
    {
      g_ioContext->run();
      g_running.store( false );
    }

    return ARINC615A_OK;
  }
  catch ( const std::exception &e )
  {
    SPDLOG_ERROR( "Exception starting ARINC 615A THA Target: {}", e.what() );
    stop();
    return ARINC615A_ERROR;
  }
}

void stop()
{
  if ( !g_running.load() && !g_ioContext )
  {
    return;
  }

  try
  {
    if ( g_findServer )
    {
      g_findServer->stop();
      g_findServer.reset();
    }
    if ( g_protocol )
    {
      g_protocol->stop();
      g_protocol.reset();
    }
    if ( g_ioContext )
    {
      g_ioContext->stop();
    }
    if ( g_workerThread && g_workerThread->joinable() )
    {
      g_workerThread->join();
      g_workerThread.reset();
    }
    g_targetOperation.reset();
    g_errorOperation.reset();
    g_ioContext.reset();
    g_running.store( false );
    SPDLOG_INFO( "ARINC 615A THA Target stopped." );
  }
  catch ( const std::exception &e )
  {
    SPDLOG_ERROR( "Exception stopping ARINC 615A THA Target: {}", e.what() );
  }
}

bool isRunning()
{
  return g_running.load();
}

TargetDataLoaderConfiguration& configuration()
{
  return g_configuration;
}

} // namespace Arinc615aTha

// C API bindings
extern "C" {

int arinc615a_tha_init_default( void )
{
  return Arinc615aTha::initDefault();
}

int arinc615a_tha_init_json( const char * json_string )
{
  if ( !json_string )
  {
    return ARINC615A_ERROR;
  }
  return Arinc615aTha::initFromJson( json_string );
}

int arinc615a_tha_init_file( const char * json_file_path )
{
  if ( !json_file_path )
  {
    return ARINC615A_ERROR;
  }
  return Arinc615aTha::initFromFile( json_file_path );
}

int arinc615a_tha_start( int run_in_background )
{
  return Arinc615aTha::start( run_in_background != 0 );
}

void arinc615a_tha_stop( void )
{
  Arinc615aTha::stop();
}

int arinc615a_tha_is_running( void )
{
  return Arinc615aTha::isRunning() ? 1 : 0;
}

void arinc615a_tha_set_log_level( int level )
{
  if ( level >= 0 && level <= 6 )
  {
    spdlog::set_level( static_cast< spdlog::level::level_enum >( level ) );
  }
}

} // extern "C"
