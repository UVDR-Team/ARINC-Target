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

#include <boost/program_options.hpp>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

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
 *
 * @param[in,out] ioContext
 *   I/O Context.
 * @param[in] error
 *   ASIO error code.
 * @param[in] signal
 *   Received signal.
 **/
static void signalHandler(
  boost::asio::io_context &ioContext,
  const boost::system::error_code &error,
  int signal );

/**
 * @brief Handles an ARINC 615A FIND Request.
 *
 * @param[in] targetsInformation
 *   Targets information to report.
 * @param[in] from
 *   Source of request.
 **/
static void findRequest(
  const Arinc615a::Find::TargetsInformation &targetsInformation,
  const boost::asio::ip::udp::endpoint &from );

/**
 * @brief Handles an ARINC 615A Operation Request.
 *
 * @param[in,out] ioContext
 *   I/O Context.
 * @param[in] configuration
 *   Target Data Loader Configuration.
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
static void operationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  Arinc615a::OperationType operation,
  const boost::asio::ip::udp::endpoint &remote,
  const ::Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Information Operation Request
 *
 * @param[in,out] ioContext
 *   I/O Context.
 * @param[in] configuration
 *   Target Data Loader Configuration.
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
static void informationOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::InformationOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Upload Operation Request
 *
 * @param[in,out] ioContext
 *   I/O Context.
 * @param[in] configuration
 *   Target Data Loader Configuration.
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
static void uploadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::UploadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Media Defined Download Operation Request
 *
 * @param[in,out] ioContext
 *   I/O Context.
 * @param[in] configuration
 *   Target Data Loader Configuration.
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
static void mediaDefinedDownloadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Operator Defined Download Operation Request
 *
 * @param[in,out] ioContext
 *   I/O Context.
 * @param[in] configuration
 *   Target Data Loader Configuration.
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
static void operatorDefinedDownloadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId );

/**
 * @brief Operation Finished handler.
 **/
static void operationFinished();

/**
 * @brief Error Operation completed handler.
 **/
static void errorOperationCompleted();

//! FIND Server
static Arinc615a::Find::Servers::ServerPtr findServer;
//! ARINC 615A Target Protocol
static Arinc615a::Target::ProtocolPtr protocol;
//! ARINC 615A Target Operation
static std::shared_ptr< Arinc615aTha::TargetOperation > targetOperation;
//! Error Operation
static Arinc615a::Target::ErrorOperationPtr errorOperation;

int main( const int argc, char * argv[] )
try
{
  spdlog::set_level( spdlog::level::level_enum::warn );

  try
  {
    boost::program_options::options_description optionsDescription{ "ARINC 615A THA Options" };

    optionsDescription.add_options()
    (
      "help",
      "print this help screen"
    );

    Arinc615aTha::TargetDataLoaderConfiguration configuration;
    optionsDescription.add( configuration.dataLoader.options() );
    optionsDescription.add( configuration.find.options() );

    std::cout << "ARINC 615A THA\n";

    // Try to load Configuration from Configuration File
    const auto configFile{ std::filesystem::path{ argv[ 0 ] }.replace_extension( ".json" ) };

    boost::property_tree::ptree properties;

    if ( std::filesystem::is_regular_file( configFile ) )
    {
      boost::property_tree::read_json( configFile.string(), properties );
    }
    else
    {
      SPDLOG_WARN(
        "No configuration file '{}' Found. Using default configuration - No operations are active.",
        configFile.string() );
    }
    configuration.fromProperties( properties );

    // Handle Command Line
    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::parse_command_line( argc, argv, optionsDescription ),
      variablesMap );

    // NOLINTNEXTLINE( readability-container-contains ):VCPKG/MSVC compiles boost programm_options without C++20 support
    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout << "ARINC 615A Test Hardware\n" << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    boost::asio::io_context ioContext;
    boost::asio::signal_set signals{ ioContext, SIGINT, SIGTERM };

    findServer = Arinc615a::Find::Servers::Server::instance( ioContext );
    assert( findServer );

    findServer
      ->requestHandler( std::bind_front( &findRequest, std::ref( configuration.findInformation ) ) )
      .localEndpoint( { configuration.find.localInterfaceAddress, configuration.find.findPort } );

    findServer->start();

    protocol = Arinc615a::Target::Protocol::instance(
      ioContext,
      Arinc615a::Target::ProtocolConfiguration{
        .newOperationRequestHandler =
          std::bind_front( &operationRequest, std::ref( ioContext ), std::ref( configuration ) ),
        .configuration = configuration.dataLoader,
        .protocolVersion = configuration.version } );
    assert( protocol );

    protocol->start();

    // connect to SIGINT and SIGTERM
    signals.async_wait( std::bind_front( &signalHandler, std::ref( ioContext ) ) );

    auto ioRunner{
      std::jthread{ [ &ioContext ]{
        ioContext.run();
      } } };

    ioRunner.join();

    return EXIT_SUCCESS;
  }
  catch ( const boost::program_options::error &e )
  {
    std::cerr << std::format(
      "Error parsing command line: {}\n"
      "Enter '{} --help' for command line description.\n",
      e.what(),
      argv[ 0 ] );
    return EXIT_FAILURE;
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

static void signalHandler(
  boost::asio::io_context &ioContext,
  const boost::system::error_code &error,
  const int signal )
{
  // handle abort
  if ( boost::asio::error::operation_aborted == error )
  {
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

  std::cout << "Termination request\n";

  findServer->stop();
  protocol->stop();

  ioContext.stop();
}

static void findRequest(
  const Arinc615a::Find::TargetsInformation &targetsInformation,
  const boost::asio::ip::udp::endpoint &from )
{
  for ( const auto &findInformation : targetsInformation )
  {
    findServer->response( from, findInformation );
  }
}

static void operationRequest(
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

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &errorOperationCompleted ),
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
   SPDLOG_ERROR( "Another operation already active" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &errorOperationCompleted ),
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

      errorOperation =
        protocol->errorOperation( Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = std::bind_front( &errorOperationCompleted ),
          .operation = operation,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
          .description = "Invalid Operation Request" } );

      errorOperation->start( remote, clientTftpOptions, port );
      return;
  }
}

static void informationOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::InformationOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  // If operation is not enabled, reject it
  if ( !opConfig.enabled )
  {
   SPDLOG_ERROR( "Information Operation not Enabled" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &errorOperationCompleted ),
        .operation = Arinc615a::OperationType::Information,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  targetOperation = std::make_shared< Arinc615aTha::TargetInformationOperation >(
    std::bind_front( &operationFinished ),
    ioContext,
    opConfig,
    *protocol,
    targetId,
    configuration.statusTransmissionRate );

  targetOperation->initialise( remote, clientTftpOptions, port );
}

static void uploadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::UploadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  // If operation is not enabled, reject it
  if ( !opConfig.enabled )
  {
   SPDLOG_ERROR( "Upload Operation not Enabled" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &errorOperationCompleted ),
        .operation = Arinc615a::OperationType::Upload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationNotSupported,
        .description = "Operation not enabled" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  if ( std::filesystem::is_directory( opConfig.directory ) )
  {
   SPDLOG_ERROR( "Upload Directory does not exist" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &errorOperationCompleted ),
        .operation = Arinc615a::OperationType::Upload,
        .targetId = targetId,
        .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
        .description = "Upload Directory does not exist" } );

    errorOperation->start( remote, clientTftpOptions, port );

    return;
  }

  targetOperation = std::make_shared< Arinc615aTha::TargetUploadOperation >(
    std::bind_front( &operationFinished ),
    ioContext,
    opConfig,
    *protocol,
    targetId,
    configuration.statusTransmissionRate );

  targetOperation->initialise( remote, clientTftpOptions, port );
}

static void mediaDefinedDownloadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  // If operation is not enabled, reject it
  if ( !opConfig.enabled )
  {
   SPDLOG_ERROR( "Operation not enabled" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &errorOperationCompleted ),
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
     SPDLOG_ERROR( "Download Directory does not exist: '{}'", directory.string() );

      errorOperation = protocol->errorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = std::bind_front( &errorOperationCompleted ),
          .operation = Arinc615a::OperationType::MediaDefinedDownload,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
          .description = "Download Directory does not exist" } );

      errorOperation->start( remote, clientTftpOptions, port );

      return;
    }
  }

  targetOperation = std::make_shared< Arinc615aTha::TargetMediaDefinedDownloadOperation >(
    std::bind_front( &operationFinished ),
    ioContext,
    opConfig,
    *protocol,
    targetId,
    configuration.statusTransmissionRate );

  targetOperation->initialise( remote, clientTftpOptions, port );
}

static void operatorDefinedDownloadOperationRequest(
  boost::asio::io_context &ioContext,
  const Arinc615aTha::TargetDataLoaderConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &remote,
  const Arinc615aTha::DownloadOperationConfiguration &opConfig,
  const Tftp::Packets::TftpOptions &clientTftpOptions,
  const std::optional< uint16_t > port,
  const Arinc615a::TargetId &targetId )
{
  // If operation is not enabled, reject it
  if ( !opConfig.enabled )
  {
   SPDLOG_ERROR( "Operation not Enabled" );

    errorOperation = protocol->errorOperation(
      Arinc615a::Target::ErrorOperationConfiguration{
        .completionHandler = std::bind_front( &errorOperationCompleted ),
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
     SPDLOG_ERROR( "Download Directory does not exist: '{}'", directory.string() );

      errorOperation = protocol->errorOperation(
        Arinc615a::Target::ErrorOperationConfiguration{
          .completionHandler = std::bind_front( &errorOperationCompleted ),
          .operation = Arinc615a::OperationType::OperatorDefinedDownload,
          .targetId = targetId,
          .status = Arinc615a::OperationAcceptanceStatusCode::OperationDenied,
          .description = "Download Directory does not exist" } );

      errorOperation->start( remote, clientTftpOptions, port );

      return;
    }
  }

  targetOperation = std::make_shared< Arinc615aTha::TargetOperatorDefinedDownloadOperation >(
    std::bind_front( &operationFinished ),
    ioContext,
    opConfig,
    *protocol,
    targetId,
    configuration.statusTransmissionRate );

  targetOperation->initialise( remote, clientTftpOptions, port );
}

static void operationFinished()
{
  SPDLOG_INFO( "Operation finished" );

  targetOperation.reset();
}

static void errorOperationCompleted()
{
  SPDLOG_INFO( "Error Operation finished" );

  errorOperation.reset();
}
