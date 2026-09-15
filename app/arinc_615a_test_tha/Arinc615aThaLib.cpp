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
#include <arinc_649/CheckValueGenerator.hpp>

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
#ifdef ARINC615A_BACKGROUND_THREAD
#include <thread>
#endif
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <fstream>
#include <iostream>

namespace {

template<class Function>
int cResult(Function function) noexcept
{
  try { return function(); }
  catch (...) { return ARINC615A_ERROR; }
}

//! Active configuration
Arinc615aTha::TargetDataLoaderConfiguration g_configuration;

//! I/O context
std::unique_ptr< boost::asio::io_context > g_ioContext;

//! Running flag
std::atomic< bool > g_running{ false };

//! Background worker thread (when runInBackground = true)
#ifdef ARINC615A_BACKGROUND_THREAD
std::unique_ptr< std::thread > g_workerThread;
#endif
std::mutex g_lifecycleMutex;
std::condition_variable g_lifecycleChanged;
bool g_runnerActive{ false };
bool g_stopping{ false };

//! FIND Server
Arinc615a::Find::Servers::ServerPtr g_findServer;

//! ARINC 615A Target Protocol
Arinc615a::Target::ProtocolPtr g_protocol;

//! ARINC 615A Target Operation
std::shared_ptr< Arinc615aTha::TargetOperation > g_targetOperation;

//! Error Operation
Arinc615a::Target::ErrorOperationPtr g_errorOperation;
std::vector<Arinc615a::Target::ErrorOperationPtr> g_errorOperations;

Arinc615a::Target::ErrorOperationPtr createErrorOperation(
  Arinc615a::Target::ErrorOperationConfiguration configuration)
{
  auto identity = std::make_shared<std::weak_ptr<Arinc615a::Target::ErrorOperation>>();
  configuration.completionHandler = [identity] {
    // Defer removal until the completion callback has returned to ASIO.
    boost::asio::post(*g_ioContext, [identity] {
      const auto completed = identity->lock();
      std::erase(g_errorOperations, completed);
      if (g_errorOperation == completed) g_errorOperation.reset();
    });
  };
  auto operation = g_protocol->errorOperation(std::move(configuration));
  *identity = operation;
  g_errorOperations.push_back(operation);
  return operation;
}

// Called with the lifecycle lock held, only after io_context::run() has exited.
void releaseResources()
{
#ifdef ARINC615A_BACKGROUND_THREAD
  if (g_workerThread && g_workerThread->joinable()) g_workerThread->join();
  g_workerThread.reset();
#endif
  g_targetOperation.reset();
  g_errorOperation.reset();
  g_errorOperations.clear();
  g_protocol.reset();
  g_findServer.reset();
  g_ioContext.reset();
}

int runLoop()
{
  int result = ARINC615A_OK;
  try { g_ioContext->run(); }
  catch (const std::exception &e)
  {
    SPDLOG_ERROR("THA event loop failed: {}", e.what());
    result = ARINC615A_ERROR;
  }
  catch (...)
  {
    SPDLOG_ERROR("THA event loop failed");
    result = ARINC615A_ERROR;
  }
  {
    std::lock_guard lock(g_lifecycleMutex);
    g_running.store(false);
    g_runnerActive = false;
  }
  g_lifecycleChanged.notify_all();
  return result;
}

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

    g_errorOperation = createErrorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
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

    g_errorOperation = createErrorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
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

      g_errorOperation = createErrorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
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

    g_errorOperation = createErrorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
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

    g_errorOperation = createErrorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .operation = Arinc615a::OperationType::Upload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    g_errorOperation->start( remote, clientTftpOptions, port );
    return;
  }

  if ( !std::filesystem::is_directory( opConfig.directory ) )
  {
    SPDLOG_ERROR( "Upload Directory does not exist" );

    g_errorOperation = createErrorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
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

    g_errorOperation = createErrorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
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

      g_errorOperation = createErrorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
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

    g_errorOperation = createErrorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
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

      g_errorOperation = createErrorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
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
  const auto completed = g_targetOperation;
  boost::asio::post(*g_ioContext, [completed] {
    if (g_targetOperation == completed) g_targetOperation.reset();
  });
}

} // namespace

namespace Arinc615aTha {

int init( const boost::property_tree::ptree &properties )
{
  std::lock_guard lock(g_lifecycleMutex);
  if (g_runnerActive || g_stopping) return ARINC615A_ALREADY_RUN;
  try
  {
    TargetDataLoaderConfiguration candidate;
    candidate.fromProperties( properties );
    for (const auto &[id, target] : candidate.targets)
    {
      if (!Arinc615a::TargetId{id}) throw std::invalid_argument("Invalid target ID: " + id);
      const auto check = [](Arinc649::CheckValueType type) {
        if (!Arinc649::CheckValueGenerator::create(type))
          throw std::invalid_argument("Configured check value is unavailable in this Boost version");
      };
      if (target.informationOperation.enabled) check(target.informationOperation.listCheckValue);
      if (target.mediaDefinedDownloadOperation.enabled) check(target.mediaDefinedDownloadOperation.checksumOption);
      if (target.operatorDefinedDownloadOperation.enabled) check(target.operatorDefinedDownloadOperation.checksumOption);
    }
    g_configuration = std::move(candidate);
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
  return init(boost::property_tree::ptree{});
}

int start(bool runInBackground)
{
#ifndef ARINC615A_BACKGROUND_THREAD
  if (runInBackground) return ARINC615A_UNSUPPORTED;
#endif
  std::unique_lock lock(g_lifecycleMutex);
  if (g_runnerActive || g_stopping) return ARINC615A_ALREADY_RUN;
  try
  {
    releaseResources();
    g_ioContext = std::make_unique<boost::asio::io_context>();
    g_findServer = Arinc615a::Find::Servers::Server::instance(*g_ioContext);
    g_findServer->requestHandler(
      std::bind_front(&findRequest, std::cref(g_configuration.findInformation)))
      .localEndpoint({g_configuration.find.localInterfaceAddress, g_configuration.find.findPort});
    g_findServer->start();
    g_protocol = Arinc615a::Target::Protocol::instance(
      *g_ioContext,
      Arinc615a::Target::ProtocolConfiguration{
        .newOperationRequestHandler =
          std::bind_front(&operationRequest, std::ref(*g_ioContext), std::cref(g_configuration)),
        .configuration = g_configuration.dataLoader,
        .protocolVersion = g_configuration.version});
    g_protocol->start();
    g_running.store(true);
    g_runnerActive = true;
#ifdef ARINC615A_BACKGROUND_THREAD
    if (runInBackground)
    {
      g_workerThread = std::make_unique<std::thread>([] { runLoop(); });
      return ARINC615A_OK;
    }
#endif
  }
  catch (const std::exception &e)
  {
    SPDLOG_ERROR("Cannot start THA: {}", e.what());
    g_running.store(false);
    g_runnerActive = false;
    releaseResources();
    return ARINC615A_ERROR;
  }
  catch (...)
  {
    g_running.store(false);
    g_runnerActive = false;
    releaseResources();
    return ARINC615A_ERROR;
  }
  lock.unlock();
  return runLoop();
}

void stop()
{
  std::unique_lock lock(g_lifecycleMutex);
  if (g_stopping)
  {
    g_lifecycleChanged.wait(lock, [] { return !g_stopping; });
    return;
  }
  g_stopping = true;
  // stop() is the only operation issued concurrently with run(). Socket,
  // protocol and operation destruction happens after every callback has exited.
  if (g_ioContext) g_ioContext->stop();
  g_lifecycleChanged.wait(lock, [] { return !g_runnerActive; });
  releaseResources();
  g_running.store(false);
  g_stopping = false;
  lock.unlock();
  g_lifecycleChanged.notify_all();
}

bool isRunning()
{
  return g_running.load();
}

TargetDataLoaderConfiguration configuration()
{
  std::lock_guard lock(g_lifecycleMutex);
  return g_configuration;
}

} // namespace Arinc615aTha

// C API bindings
extern "C" {

int arinc615a_tha_init_default( void )
{
  return cResult([] { return Arinc615aTha::initDefault(); });
}

int arinc615a_tha_init_json( const char * json_string )
{
  if ( !json_string )
  {
    return ARINC615A_ERROR;
  }
  return cResult([&] { return Arinc615aTha::initFromJson( json_string ); });
}

int arinc615a_tha_init_file( const char * json_file_path )
{
  if ( !json_file_path )
  {
    return ARINC615A_ERROR;
  }
  return cResult([&] { return Arinc615aTha::initFromFile( json_file_path ); });
}

int arinc615a_tha_start( int run_in_background )
{
  return cResult([&] { return Arinc615aTha::start( run_in_background != 0 ); });
}

void arinc615a_tha_stop( void )
{
  try { Arinc615aTha::stop(); }
  catch (...) { SPDLOG_ERROR("THA stop failed; do not unload the module"); }
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
