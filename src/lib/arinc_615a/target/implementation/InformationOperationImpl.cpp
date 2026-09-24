// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Target::InformationOperationImpl.
 **/

#include "InformationOperationImpl.hpp"

#include <arinc_615a/target/InformationOperationHandler.hpp>
#include <arinc_615a/target/OperationConfiguration.hpp>

#include <arinc_615a/files/information/InformationOperationStatusFile.hpp>
#include <arinc_615a/files/information/LoadConfigurationListFile.hpp>

#include <arinc_615a/information/TargetHardware.hpp>
#include <arinc_615a/information/PartNumber.hpp>

#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/servers/Server.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>

#include <arinc_checksum/CheckValueGenerator.hpp>

#include <tftp/files/MemoryFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/exception/all.hpp>

namespace Arinc615a::Target {

InformationOperationImpl::InformationOperationImpl(
  boost::asio::io_context &ioContext,
  Arinc615aConfiguration dlConfiguration,
  Arinc615aVersion protocolVersion,
  Tftp::Servers::ServerPtr tftpServer,
  InformationOperationConfiguration operationConfiguration ) :
  OperationImpl{
    ioContext,
    std::move( dlConfiguration ),
    protocolVersion,
    std::move( tftpServer ),
    operationConfiguration.handler,
    std::move( operationConfiguration.targetId ),
    operationConfiguration.statusTransmissionRate },
  handler{ operationConfiguration.handler }
{
  protocolFileLogger()
    .operation( OperationType::Information )
    .loggingEnabled( configuration().protocolFileLogging );
}

void InformationOperationImpl::start(
  boost::asio::ip::udp::endpoint hostAddress,
  ::Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  doStart( std::move( hostAddress ), std::move( clientTftpOptions ), port );
}

void InformationOperationImpl::accepted()
{
  // set the status
  status( StatusCode::OperationAccepted );

  // force status transmission
  statusFile();
}

void InformationOperationImpl::inProgress(
  const bool immediateTransmission,
  const int16_t estimatedTime,
  std::string description )
{
  // set the status
  status(
    description.empty() ? StatusCode::OperationInProgress : StatusCode::OperationInProgressAdditionalInfo,
    std::move( description ),
    estimatedTime );

  // If immediate transmission is requested
  if ( immediateTransmission )
  {
    // force status transmission
    statusFile();
  }
}

void InformationOperationImpl::finished( const FinalStatus finalStatus, std::string description )
{
  // set error status
  status( statusCode( finalStatus ), std::move( description ) );

  // force status transmission
  statusFile();
}

void InformationOperationImpl::targetInformation(
  Information::TargetsHardware targetsHardware,
  const ArincChecksum::CheckValueType checkValueType )
{
  ARINC_LOG_INFO( "Send target information" );

  const Files::LoadConfigurationListFile configList{ protocolVersion(), std::move( targetsHardware ) };

  const auto rawFile{ std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< ArincSupport::RawData >( configList ) ) };

  const auto checkValueGenerator{ ArincChecksum::CheckValueGenerator::create( checkValueType ) };

  checkValueGenerator->process( ArincSupport::RawData_asRawData( rawFile->data() ) );

  // send the protocol file
  targetInformationOperation = protocolFileOperation(
    Files::ProtocolFileType::LoadConfigurationList,
    {}, // Operation deferred is ignored
    ArincSupport::bindFront( &InformationOperationImpl::targetInformationCompleted, this ),
    std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< ArincSupport::RawData >( configList ) ),
    checkValueGenerator->checkValue() );
  assert( targetInformationOperation );

  targetInformationOperation->request();
}

void InformationOperationImpl::tftpRequest(
  boost::asio::ip::udp::endpoint remote,
  const Tftp::RequestType requestType [[maybe_unused]],
  std::string filename [[maybe_unused]],
  ::Tftp::Packets::TftpOptions clientTftpOptions [[maybe_unused]],
  Tftp::Arinc615aOptions clientArinc615aOptions [[maybe_unused]] )
{
  ARINC_LOG_ERROR( "No TFTP request expected for information operation." );

  try
  {
    tftpServerErrorOperation( remote, ::Tftp::Packets::ErrorCode::FileNotFound, "RRQ/WRQ not expected" );
  }
  catch ( const Arinc615aException &e )
  {
    ARINC_LOG_ERROR( "Error send TFTP error: {}", boost::diagnostic_information( e ) );
  }
}

void InformationOperationImpl::status( const StatusCode code, std::string description, const int16_t estimatedTime )
{
  std::unique_lock lock( statusMutex );
  statusV.set( code, std::move( description ) );
  statusV.estimatedTime( estimatedTime );
}

void InformationOperationImpl::statusFile()
{
  std::unique_lock lock{ statusMutex };
  // Never replace an active transfer: destroying it invokes its completion
  // callback and can reset the replacement or finalise this operation.
  if (statusOperation) {
    statusTransmissionPending = true;
    return;
  }
  statusTransmissionPending = false;

  // if status has not been sent previously, send accepted status, set Accepted Status
  if ( Arinc615a::StatusCode::Invalid == statusV.code() )
  {
    ARINC_LOG_INFO( "Force sending of acceptance status" );
    statusV.code( Arinc615a::StatusCode::OperationAccepted );
  }

  const Files::InformationOperationStatusFile statusFile{ protocolVersion(), statusV };

  const auto file{ std::make_shared< ::Tftp::Files::MemoryFile >( static_cast< ArincSupport::RawData >( statusFile ) ) };
  assert( file );

  protocolFileLogger().transmitProtocolFile(
    protocolFilename( Files::ProtocolFileType::LoadConfigurationStatus ),
    file->data() );

  if ( statusOperation )
  {
    ARINC_LOG_WARN( "Status Operation not empty" );
  }
  lock.unlock();

  // send the protocol file
  statusOperation = protocolFileOperation(
    Files::ProtocolFileType::LoadConfigurationStatus,
    {}, // Operation deferred is ignored
    ArincSupport::bindFront( &InformationOperationImpl::statusFileCompleted, this, statusV ),
    file );
  assert( statusOperation );

  statusOperation->request();

  // increment counter for next status file transmission
  statusV.incrementCounter();

  triggerStatusTransmissionTimer();
}

void InformationOperationImpl::statusFileCompleted(
  Information::InformationStatus sentStatus,
  const Tftp::TransferStatus transferStatus )
{
  statusOperation.reset();

  ARINC_LOG_INFO( "Send status completed" );

  switch ( transferStatus )
  {

    case Tftp::TransferStatus::Successful:
      break;

    default:
      ARINC_LOG_ERROR( "Sending of status failed" );

      // finalise operation
      finalise( FinalStatus::AbortedByTargetHardware, "Status transmission failed" );

      return;
  }

  // call handler to inform
  handler.status( sentStatus );

  if (sentStatus.code() == StatusCode::OperationAccepted ||
      sentStatus.code() == StatusCode::OperationInProgress ||
      sentStatus.code() == StatusCode::OperationInProgressAdditionalInfo) {
    std::unique_lock lock{statusMutex};
    if (statusTransmissionPending || statusV.code() != sentStatus.code()) {
      lock.unlock();
      triggerStatusTransmission();
      return;
    }
  }

  switch ( sentStatus.code() )
  {

    // First transmission of status -> set to in progress
    case StatusCode::OperationAccepted:
      ARINC_LOG_INFO( "Force sending of in progress status" );

      // set in progress status and immediate transmit
      inProgress( true );
      break;

    case StatusCode::OperationInProgress:
    case StatusCode::OperationInProgressAdditionalInfo:
      triggerStatusTransmissionTimer();
      break;

    default:
      // finalise operation
      finalise( finalStatus( sentStatus.code() ), sentStatus.description() );
      break;
  }
}

void InformationOperationImpl::targetInformationCompleted( const Tftp::TransferStatus transferStatus )
{
  targetInformationOperation.reset();

  ARINC_LOG_INFO( "Send target information completed" );

  if ( transferStatus != Tftp::TransferStatus::Successful )
  {
    finished( FinalStatus::AbortedByTargetHardware, "Target Information transmission failed" );
    return;
  }

  if ( StatusCode::Invalid == statusV.code() )
  {
    // finalise operation
    finalise();
  }
  else
  {
    // if the status has been sent previously, send the completed status

    // set final status
    status( StatusCode::OperationCompleted );

    // send final status - finalise called implicitly
    statusFile();
  }
}

}
