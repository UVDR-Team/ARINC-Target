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
 * @brief Definition of Class TargetMediaDefinedDownloadOperation.
 **/

#include <arinc_support/Support.hpp>

#include "TargetMediaDefinedDownloadOperation.hpp"

#include "TargetDataLoaderConfiguration.hpp"

#include <arinc_615a/information/DownloadStatus.hpp>

#include <arinc_615a/target/MediaDefinedDownloadOperation.hpp>
#include <arinc_615a/target/Protocol.hpp>
#include <arinc_615a/target/OperationConfiguration.hpp>

#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <arinc_checksum/CheckValueGenerator.hpp>

#include <tftp/files/StreamFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/clients/Operation.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <arinc_support/Format.hpp>
#include <fstream>

namespace Arinc615aTha {

TargetMediaDefinedDownloadOperation::TargetMediaDefinedDownloadOperation(
  Finished finished,
  boost::asio::io_context &ioContext,
  const DownloadOperationConfiguration &configuration,
  Arinc615a::Target::Protocol &protocol,
  Arinc615a::TargetId targetId,
  std::chrono::seconds statusTransmissionRate ) :
  TargetOperation{ std::move( finished ), ioContext },
  configurationV{ configuration },
  operationV{ protocol.mediaDefinedDownloadOperation(
    Arinc615a::Target::MediaDefinedDownloadOperationConfiguration{
      .handler = *this,
      .targetId = std::move( targetId ),
      .statusTransmissionRate = statusTransmissionRate } ) }
{
  ARINC_LOG_INFO( "Create Media Defined Download Operation" );

  for ( auto const &directory : configurationV.directories )
  {
    ARINC_LOG_INFO( "Directory: {}", directory.string() );

    std::error_code errorCode{};
    for ( const auto &file : std::filesystem::recursive_directory_iterator(
      directory,
      std::filesystem::directory_options::skip_permission_denied,
      errorCode ) )
    {
      if ( file.is_regular_file() )
      {
        ARINC_LOG_INFO( "Available file: {}", file.path().string() );

        availableFilesV.try_emplace( file.path().filename().string(), file.path() );
      }
    }
  }
}

TargetMediaDefinedDownloadOperation::~TargetMediaDefinedDownloadOperation() = default;

void TargetMediaDefinedDownloadOperation::initialise(
  boost::asio::ip::udp::endpoint hostAddress,
  Tftp::Packets::TftpOptions clientTftpOptions,
  std::optional< uint16_t > port )
{
  assert( operationV );

  try
  {
    std::for_each(
      configurationV.directories.begin(), configurationV.directories.end(),
      []( auto &directory )
      {
        std::error_code errorCode{};
        std::filesystem::create_directories( directory, errorCode );
      } );

    operationV->start( std::move( hostAddress ), std::move( clientTftpOptions ), port );
  }
  catch ( const boost::exception &e )
  {
    ARINC_LOG_ERROR( "Error during Media Define Download operation: {}", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    ARINC_LOG_ERROR( "Error during Media Define Download operation: {}", boost::diagnostic_information( e ) );
  }
  catch ( ... )
  {
    ARINC_LOG_ERROR( "Error during Media Define Download operation" );
  }
}

void TargetMediaDefinedDownloadOperation::abort()
{
  assert( operationV );

  operationV->filesFinished( Arinc615a::FinalStatus::AbortedByTargetHardware, "THA Abort" );
  operationV->inProgress( true, -1, "THA Abort in progress" );

  if ( fileOperationV )
  {
    fileOperationV->gracefulAbort( Tftp::Packets::ErrorCode::NotDefined, "THA Abort" );
  }

  // TODO move to completion routine.
  operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "THA Abort" );
}

void TargetMediaDefinedDownloadOperation::initialised()
{
  assert( operationV );

  // send accepted status
  operationV->accepted();
}

void TargetMediaDefinedDownloadOperation::finished(
  const Arinc615a::FinalStatus finalStatus,
  std::string_view description )
{
  ARINC_LOG_INFO(
    "Operation finished: {} '{}'",
    Arinc615a::StatusCodeDescription::instance().name( Arinc615a::statusCode( finalStatus ) ),
    description );
  finished();
}

void TargetMediaDefinedDownloadOperation::abortRequest( const Arinc615a::AbortRequest abortRequest )
{
  ARINC_LOG_INFO( "Abort request from host" );

  abortRequestPending = abortRequest;

  operationV->filesFinished( Arinc615a::FinalStatus::AbortedByTargetHardware, "Abort by Host" );

  if ( fileOperationV )
  {
    operationV->inProgress( true, -1, "Host Abort in progress" );
    fileOperationV->gracefulAbort( Tftp::Packets::ErrorCode::NotDefined, "THA Abort" );
  }
  else
  {
    operationV->finished( Arinc615a::finalStatus( abortRequest ), "Abort by Host" );
  }
}

void TargetMediaDefinedDownloadOperation::status( const Arinc615a::Information::DownloadStatus &status )
{
  ARINC_LOG_INFO(
    "Status:\n"
    "\tCounter:         {}\n"
    "\tStatus:          {} ({})\n"
    "\tDescription:     '{}'\n"
    "\tException Timer: {}\n"
    "\tEstimated Time:  {}\n",
    status.counter(),
    Arinc615a::StatusCodeDescription::instance().name( status.code() ),
    ArincSupport::toUnderlying( status.code() ),
    status.description(),
    status.exceptionTimer(),
    status.estimatedTime() );

  if ( Arinc615a::StatusCode::OperationAccepted == status.code() )
  {
    // set in progress
    operationV->inProgress( true );
  }
}

void TargetMediaDefinedDownloadOperation::downloadingRequest(
  const Arinc615a::Information::DownloadFiles &files,
  [[maybe_unused]] ArincSupport::ConstRawDataSpan userDefinedData )
{
  ARINC_LOG_INFO( "Download Request: {} files", files.size() );

  if ( files.empty() )
  {
    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "Empty Downloading Request" );
    return;
  }

  filesV = files;
  currentFileV = filesV.begin();

  // Prepare status
  for ( const auto &file : files )
  {
    operationV->fileAccepted( file );
  }

  operationV->inProgress( false );

  boost::asio::post( ioContext(), ArincSupport::bindFront( &TargetMediaDefinedDownloadOperation::sendFile, this ) );
}

void TargetMediaDefinedDownloadOperation::sendFile()
{
  ARINC_LOG_INFO( "Send file {}", *currentFileV );

  auto const fileInfo{ availableFilesV.find( *currentFileV ) };
  if ( availableFilesV.end() == fileInfo )
  {
    ARINC_LOG_ERROR( "File Info not found" );

    operationV->fileFinished(
      *currentFileV,
      Arinc615a::FinalStatus::LoadPartNumberOrDownloadFileFailed,
      "File Info not found" );
    ++currentFileV;

    operationV->filesFinished( Arinc615a::FinalStatus::AbortedByTargetHardware, "Aborted: File Info not found" );
    operationV->inProgress( false , -1, "Abort in Progress" );
    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "File Info not found" );

    return;
  }

  // update status
  operationV->fileInProgress( *currentFileV );
  operationV->inProgress( true );

  auto fileStream{
    std::make_shared< Tftp::Files::StreamFile >( Tftp::Files::StreamFile::Operation::Transmit, fileInfo->second ) };

  auto partNumber{ configurationV.partNumberOption ? fileInfo->first : std::string{} };
  auto checkValue{
    ( ArincChecksum::CheckValueType::NotUsed == configurationV.checksumOption )
      ? ArincChecksum::CheckValue::NoCheckValue
      : ArincChecksum::CheckValueGenerator::checkValue( configurationV.checksumOption, fileInfo->second ) };

  fileOperationV = operationV->transferFile(
    ArincSupport::bindFront( &TargetMediaDefinedDownloadOperation::fileOptionsNegotiation, this, partNumber, checkValue ),
    ArincSupport::bindFront( &TargetMediaDefinedDownloadOperation::fileCompleted, this ),
    fileStream,
    *currentFileV,
    partNumber,
    checkValue );
  assert( fileOperationV );

  fileOperationV->request();
}

bool TargetMediaDefinedDownloadOperation::fileOptionsNegotiation(
  std::string_view providedPartNumber,
  const ArincChecksum::CheckValue &providedCheckValue,
  std::string_view partNumber,
  const ArincChecksum::CheckValue &checksum )
{
  if ( providedPartNumber.empty() )
  {
    if ( !partNumber.empty() )
    {
      ARINC_LOG_ERROR( "Host sent Part Number Option which was not advertised" );
      return false;
    }
  }
  else
  {
    if ( partNumber.empty() )
    {
      ARINC_LOG_WARN( "Host has not acknowledged Part Number Option" );
    }
    else if ( providedPartNumber != partNumber )
    {
      ARINC_LOG_ERROR( "Received Part Number Option differs from sent one" );

      return false;
    }
    else
    {
      // nothing to do
    }
  }

  if ( ArincChecksum::CheckValue::NoCheckValue == providedCheckValue )
  {
    if ( ArincChecksum::CheckValue::NoCheckValue != checksum )
    {
      ARINC_LOG_ERROR( "Host sent Checksum Option which was not advertised" );
      return false;
    }
  }
  else
  {
    if ( ArincChecksum::CheckValue::NoCheckValue == checksum )
    {
      ARINC_LOG_WARN( "Host has not acknowledged Checksum Option" );
    }
    else if ( checksum != providedCheckValue )
    {
      ARINC_LOG_ERROR(
        "Received Checksum Option differs from sent one: RX: {} TX: {}",
        checksum.format(),
        providedCheckValue.format() );
      return false;
    }
    else
    {
      // nothing to do
    }
  }

  return true;
}

void TargetMediaDefinedDownloadOperation::fileCompleted( const Arinc615a::Tftp::TransferStatus status )
{
  if ( abortRequestPending )
  {
    operationV->finished( Arinc615a::finalStatus( *abortRequestPending ), "Abort by Host" );
    return;
  }

  if ( Arinc615a::Tftp::TransferStatus::Successful != status )
  {
    ARINC_LOG_ERROR( "Transfer Error" );

    operationV->fileFinished(
      *currentFileV,
      Arinc615a::FinalStatus::LoadPartNumberOrDownloadFileFailed,
      "Transfer Error" );

    // update status
    operationV->inProgress( true );

    ++currentFileV;

    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "File transfer Error" );

    return;
  }

  operationV->filesFinished( Arinc615a::FinalStatus::Completed );
  ++currentFileV;

  // update status
  operationV->inProgress( true );

  if ( currentFileV == filesV.end() )
  {
    operationV->finished( Arinc615a::FinalStatus::Completed );
    return;
  }

  // send the next file
  boost::asio::post( ioContext(), ArincSupport::bindFront( &TargetMediaDefinedDownloadOperation::sendFile, this ) );
}

}
