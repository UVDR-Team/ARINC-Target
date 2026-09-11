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
 * @brief Definition of Class TargetOperatorDefinedDownloadOperation.
 **/

#include "TargetOperatorDefinedDownloadOperation.hpp"

#include "TargetDataLoaderConfiguration.hpp"

#include <arinc_615a/information/DownloadStatus.hpp>
#include <arinc_615a/information/DownloadFileInformation.hpp>

#include <arinc_615a/target/OperatorDefinedDownloadOperation.hpp>
#include <arinc_615a/target/Protocol.hpp>
#include <arinc_615a/target/OperationConfiguration.hpp>

#include <arinc_615a/tftp/clients/WriteOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <arinc_649/CheckValueGenerator.hpp>

#include <tftp/files/StreamFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/clients/Operation.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/diagnostic_information.hpp>

#include <format>
#include <fstream>

namespace Arinc615aTha {

TargetOperatorDefinedDownloadOperation::TargetOperatorDefinedDownloadOperation(
  Finished finished,
  boost::asio::io_context &ioContext,
  const DownloadOperationConfiguration &configuration,
  Arinc615a::Target::Protocol &protocol,
  Arinc615a::TargetId targetId,
  std::chrono::seconds statusTransmissionRate ) :
  TargetOperation{ std::move( finished ), ioContext },
  configurationV{ configuration },
  operationV{ protocol.operatorDefinedDownloadOperation(
    Arinc615a::Target::OperatorDefinedDownloadOperationConfiguration{
      .handler = *this,
      .targetId = std::move( targetId ),
      .statusTransmissionRate = statusTransmissionRate } ) }
{
  SPDLOG_INFO( "Create Operator Defined Download Operation" );

  for ( auto const &directory : configurationV.directories )
  {
    SPDLOG_INFO( "Directory: {}", directory.string() );

    std::error_code errorCode{};
    for ( const auto &file : std::filesystem::recursive_directory_iterator(
      directory,
      std::filesystem::directory_options::skip_permission_denied,
      errorCode ) )
    {
      if ( file.is_regular_file() )
      {
        SPDLOG_INFO( "Available file: {}", file.path().string() );

        availableFilesV.try_emplace( file.path().filename().string(), file.path() );
      }
    }
  }
}

TargetOperatorDefinedDownloadOperation::~TargetOperatorDefinedDownloadOperation() = default;

void TargetOperatorDefinedDownloadOperation::initialise(
  boost::asio::ip::udp::endpoint hostAddress,
  Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  assert( operationV );

  try
  {
    operationV->start( std::move( hostAddress ), std::move( clientTftpOptions ), port );
  }
  catch ( const boost::exception &e )
  {
    SPDLOG_ERROR( "Error during Operator Define Download operation: {}", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    SPDLOG_ERROR( "Error during Operator Define Download operation: {}", boost::diagnostic_information( e ) );
  }
  catch ( ... )
  {
    SPDLOG_ERROR( "Error during Operator Define Download operation" );
  }
}

void TargetOperatorDefinedDownloadOperation::abort()
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

void TargetOperatorDefinedDownloadOperation::initialised()
{
  assert( operationV );

  // send accepted status
  operationV->accepted();
}

void TargetOperatorDefinedDownloadOperation::finished(
  const Arinc615a::FinalStatus finalStatus,
  std::string_view description )
{
  SPDLOG_INFO(
    "Operation finished: {} '{}'",
    Arinc615a::StatusCodeDescription::instance().name( Arinc615a::statusCode( finalStatus ) ),
    description );
  finished();
}

void TargetOperatorDefinedDownloadOperation::abortRequest( const Arinc615a::AbortRequest abortRequest )
{
  SPDLOG_INFO( "Abort request from host" );

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

void TargetOperatorDefinedDownloadOperation::status( const Arinc615a::Information::DownloadStatus &status )
{
  SPDLOG_INFO(
    "Status:\n"
    "\tCounter:         {}\n"
    "\tStatus:          {} ({})\n"
    "\tDescription:     '{}'\n"
    "\tException Timer: {}\n"
    "\tEstimated Time:  {}\n",
    status.counter(),
    Arinc615a::StatusCodeDescription::instance().name( status.code() ),
    std::to_underlying( status.code() ),
    status.description(),
    status.exceptionTimer(),
    status.estimatedTime() );

  if ( !fileListSent )
  {
    boost::asio::post( ioContext(), std::bind_front( &TargetOperatorDefinedDownloadOperation::sendLoadList, this ) );
    fileListSent = true;
  }
}

void TargetOperatorDefinedDownloadOperation::downloadingAnswer( const Arinc615a::Information::DownloadFiles &files )
{
  SPDLOG_INFO( "Download Answers: {} files", files.size() );

  if ( files.empty() )
  {
    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "Empty Downloading Answer" );
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

  boost::asio::post( ioContext(), std::bind_front( &TargetOperatorDefinedDownloadOperation::sendFile, this ) );
}

void TargetOperatorDefinedDownloadOperation::sendLoadList()
{
  Arinc615a::Information::DownloadFilesInformation files;

  for ( const auto &[ filename, filePath ] : availableFilesV )
  {
    files.emplace_back( Arinc615a::Information::DownloadFileInformation{
      .filename = filename,
      .description = std::format( "Size: {}", std::filesystem::file_size( filePath ) ) } );
  }

  if ( files.empty() )
  {
    SPDLOG_ERROR( "No files available" );

    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "No files available" );
    return;
  }

  operationV->downloadingList( std::move( files ) );
}

void TargetOperatorDefinedDownloadOperation::sendFile()
{
  SPDLOG_INFO( "Send file {}", *currentFileV );

  auto const fileInfo{ availableFilesV.find( *currentFileV ) };
  if ( availableFilesV.end() == fileInfo )
  {
    SPDLOG_ERROR( "File Info not found" );

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
    ( Arinc649::CheckValueType::NotUsed == configurationV.checksumOption )
      ? Arinc649::CheckValue::NoCheckValue
      : Arinc649::CheckValueGenerator::checkValue( configurationV.checksumOption, fileInfo->second ) };

  fileOperationV = operationV->transferFile(
    std::bind_front( &TargetOperatorDefinedDownloadOperation::fileOptionsNegotiation, this, partNumber, checkValue ),
    std::bind_front( &TargetOperatorDefinedDownloadOperation::fileCompleted, this ),
    fileStream,
    *currentFileV,
    partNumber,
    checkValue );
  assert( fileOperationV );

  fileOperationV->request();
}

bool TargetOperatorDefinedDownloadOperation::fileOptionsNegotiation(
  std::string_view providedPartNumber,
  const Arinc649::CheckValue &providedCheckValue,
  std::string_view partNumber,
  const Arinc649::CheckValue &checksum )
{
  if ( providedPartNumber.empty() )
  {
    if ( !partNumber.empty() )
    {
      SPDLOG_ERROR( "Host sent Part Number Option which was not advertised" );
      return false;
    }
  }
  else
  {
    if ( partNumber.empty() )
    {
      SPDLOG_WARN( "Host has not acknowledged Part Number Option" );
    }
    else if ( providedPartNumber != partNumber )
    {
      SPDLOG_ERROR( "Received Part Number Option differs from sent one" );

      return false;
    }
    else
    {
      // nothing to do
    }
  }

  if ( Arinc649::CheckValue::NoCheckValue == providedCheckValue )
  {
    if ( Arinc649::CheckValue::NoCheckValue != checksum )
    {
      SPDLOG_ERROR( "Host sent Checksum Option which was not advertised" );
      return false;
    }
  }
  else
  {
    if ( Arinc649::CheckValue::NoCheckValue == checksum )
    {
      SPDLOG_WARN( "Host has not acknowledged Checksum Option" );
    }
    else if ( checksum != providedCheckValue )
    {
      SPDLOG_ERROR(
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

void TargetOperatorDefinedDownloadOperation::fileCompleted( const Arinc615a::Tftp::TransferStatus status )
{
  if ( abortRequestPending )
  {
    operationV->finished( Arinc615a::finalStatus( *abortRequestPending ), "Abort by Host" );
    return;
  }

  if ( Arinc615a::Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Transfer Error" );

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
  boost::asio::post( ioContext(), std::bind_front( &TargetOperatorDefinedDownloadOperation::sendFile, this ) );
}

}
