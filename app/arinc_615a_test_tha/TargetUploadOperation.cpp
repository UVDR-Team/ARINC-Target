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
 * @brief Definition of Class TargetUploadOperation.
 **/

#include "TargetUploadOperation.hpp"
#include <helper/Underlying.hpp>

#include <arinc_615a/information/UploadStatus.hpp>

#include <arinc_615a/target/UploadOperation.hpp>
#include <arinc_615a/target/Protocol.hpp>
#include <arinc_615a/target/OperationConfiguration.hpp>

#include <arinc_615a/tftp/clients/ReadOperation.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <arinc_665/files/LoadHeaderFile.hpp>

#include <tftp/files/MemoryFile.hpp>
#include <tftp/files/StreamFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/clients/Operation.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/diagnostic_information.hpp>

namespace Arinc615aTha {

TargetUploadOperation::TargetUploadOperation(
  Finished finished,
  boost::asio::io_context &ioContext,
  const UploadOperationConfiguration &configuration,
  Arinc615a::Target::Protocol &protocol,
  Arinc615a::TargetId targetId,
  std::chrono::seconds statusTransmissionRate ) :
  TargetOperation{ std::move( finished ), ioContext },
  configurationV{ configuration },
  operationV{ protocol.uploadOperation(
    Arinc615a::Target::UploadOperationConfiguration{
      .handler = *this,
      .targetId = std::move( targetId ),
      .statusTransmissionRate = statusTransmissionRate } ) }
{
}

TargetUploadOperation::~TargetUploadOperation() = default;

void TargetUploadOperation::initialise(
  boost::asio::ip::udp::endpoint hostAddress,
  Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  assert( operationV );

  try
  {
    std::error_code errorCode{};
    std::filesystem::create_directories( configurationV.directory, errorCode );

    operationV->start( std::move( hostAddress ), std::move( clientTftpOptions ), port );
  }
  catch ( const boost::exception &e )
  {
    SPDLOG_ERROR( "Error during Upload operation: {}", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    SPDLOG_ERROR( "Error during Upload operation: {}", boost::diagnostic_information( e ) );
  }
  catch ( ... )
  {
    SPDLOG_ERROR( "Error during Upload operation" );
  }
}

void TargetUploadOperation::abort()
{
  assert( operationV );

  operationV->loadsFinished( Arinc615a::FinalStatus::AbortedByTargetHardware, "THA Abort" );
  operationV->inProgress( true, -1, "THA Abort in progress" );

  if ( fileOperationV )
  {
    fileOperationV->gracefulAbort( Tftp::Packets::ErrorCode::NotDefined, "THA Abort" );
  }

  if ( loadHeaderOperationV )
  {
    loadHeaderOperationV->gracefulAbort( Tftp::Packets::ErrorCode::NotDefined, "THA Abort" );
  }

  // TODO move to completion routine.
  operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "THA Abort" );
}

void TargetUploadOperation::initialised()
{
  assert( operationV );

  // send accepted status
  operationV->accepted();
}

void TargetUploadOperation::finished( const Arinc615a::FinalStatus finalStatus, std::string_view description )
{
  SPDLOG_INFO(
    "Operation finished: {} '{}'",
    Arinc615a::StatusCodeDescription::instance().name( Arinc615a::statusCode( finalStatus ) ),
    description );
  finished();
}

void TargetUploadOperation::abortRequest( const Arinc615a::AbortRequest abortRequest )
{
  SPDLOG_INFO( "Abort Request from host" );

  operationV->loadsFinished( Arinc615a::FinalStatus::AbortedByTargetHardware, "Abort by Host" );
  operationV->inProgress( true, -1, "Host Abort in progress" );

  if ( fileOperationV )
  {
    fileOperationV->gracefulAbort( Tftp::Packets::ErrorCode::NotDefined, "THA Abort" );
  }

  if ( loadHeaderOperationV )
  {
    loadHeaderOperationV->gracefulAbort( Tftp::Packets::ErrorCode::NotDefined, "THA Abort" );
  }

  // TODO Move to completion routines
  operationV->finished( Arinc615a::finalStatus( abortRequest ) );
}

void TargetUploadOperation::status( const Arinc615a::Information::UploadStatus &status )
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
    Helper::toUnderlying( status.code() ),
    status.description(),
    status.exceptionTimer(),
    status.estimatedTime() );
}

void TargetUploadOperation::loadList( const Arinc615a::Information::UploadLoads &loads )
{
  SPDLOG_INFO( "Upload Load List - {} load headers", loads.size() );

  if ( loads.empty() )
  {
    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "Empty Downloading Answer" );
    return;
  }

  loadsV = loads;
  currentLoadV = loadsV.begin();

  // Add loads status
  for ( const auto &load : loads )
  {
    operationV->loadAccepted( load.headerFilename, load.partNumber );
  }

  operationV->inProgress( true, -1, {}, Arinc615a::Information::Ratio{ 0 } );

  boost::asio::post( ioContext(), std::bind_front( &TargetUploadOperation::receiveLoadHeader, this ) );
}

void TargetUploadOperation::receiveLoadHeader()
{
  SPDLOG_INFO( "Load '{}' '{}'", currentLoadV->headerFilename, currentLoadV->partNumber );

  assert( currentLoadV != loadsV.end() );

  auto file{ std::make_shared< Tftp::Files::MemoryFile >() };
  auto partNumber{ configurationV.partNumberOption ? currentLoadV->partNumber : std::string{} };

  loadHeaderOperationV = operationV->transferFile(
    std::bind_front( &TargetUploadOperation::uploadHeaderFileOptionsNegotiation, this, partNumber ),
    std::bind_front( &TargetUploadOperation::uploadHeaderFileCompleted, this, file ),
    file,
    currentLoadV->headerFilename,
    partNumber,
    // We cannot know any check code of Load Header
    Arinc649::CheckValue::NoCheckValue );

  assert( loadHeaderOperationV );

  loadHeaderOperationV->request();
}

bool TargetUploadOperation::uploadHeaderFileOptionsNegotiation(
  std::string_view providedPartNumber,
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
    else if ( partNumber != providedPartNumber )
    {
      SPDLOG_ERROR( "Received Part Number Option differs from sent one" );

      return false;
    }
    else
    {
      // nothing to do
    }
  }

  if ( Arinc649::CheckValue::NoCheckValue != checksum )
  {
    SPDLOG_ERROR( "Received Checksum Option when no one sent" );

    return false;
  }

  return true;
}

void TargetUploadOperation::uploadHeaderFileCompleted(
  Tftp::Files::MemoryFilePtr loadHeader,
  const Arinc615a::Tftp::TransferStatus status )
{
  loadHeaderOperationV.reset();

  if ( Arinc615a::Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Transfer Error" );

    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "File transfer Error" );
    return;
  }

  Arinc665::Files::LoadHeaderFile loadHeaderFile{ loadHeader->data() };

  SPDLOG_INFO( "Upload Load Header ", loadHeaderFile.partNumber() );

  if ( loadHeaderFile.dataFiles().empty() )
  {
    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "Load Header does not contain data files" );
  }

  filesV = loadHeaderFile.dataFiles();
  filesV.insert( filesV.end(), loadHeaderFile.supportFiles().begin(), loadHeaderFile.supportFiles().end() );
  currentFileV = filesV.begin();

  // update status
  operationV->loadInProgress( currentLoadV->headerFilename, Arinc615a::Information::Ratio{ 0 } );
  operationV->inProgress( false, -1, {}, Arinc615a::Information::Ratio{ 0 } );

  // receive file
  boost::asio::post( ioContext(), std::bind_front( &TargetUploadOperation::receiveFile, this ) );
}

void TargetUploadOperation::receiveFile()
{
  assert( currentFileV != filesV.end() );

  if ( fileOperationV )
  {
    SPDLOG_WARN( "File Operation not empty" );
  }

  SPDLOG_INFO( "Receive file {}", currentFileV->filename );

  auto file{ std::make_shared< Tftp::Files::StreamFile >(
    Tftp::Files::StreamFile::Operation::Receive,
    configurationV.directory / currentFileV->filename ) };
  assert( file );

  auto partNumber{ configurationV.partNumberOption ? currentLoadV->partNumber : std::string{} };
  auto checkValue{
    configurationV.checksumOption
      ? Arinc649::CheckValue::crc16( currentFileV->crc )
      : Arinc649::CheckValue::NoCheckValue };

  fileOperationV = operationV->transferFile(
    std::bind_front( &TargetUploadOperation::fileOptionsNegotiation, this, partNumber, checkValue ),
    std::bind_front( &TargetUploadOperation::fileCompleted, this, file ),
    file,
    currentFileV->filename,
    partNumber,
    checkValue );
  assert( fileOperationV );

  fileOperationV->request();
}

bool TargetUploadOperation::fileOptionsNegotiation(
  std::string_view providedPartNumber,
  const Arinc649::CheckValue &providedCheckValue,
  std::string_view partNumber,
  const Arinc649::CheckValue &checksum )
{
  if ( providedPartNumber.empty() )
  {
    if ( !partNumber.empty() )
    {
      SPDLOG_ERROR( "Host sent Part Number Option wich was not advertised" );
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
      SPDLOG_ERROR( "Host sent Checksum Option wich was not advertised" );
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

void TargetUploadOperation::fileCompleted( Tftp::Files::StreamFilePtr file, Arinc615a::Tftp::TransferStatus status )
{
  // file will be used in feature to validate checksum
  (void)file;

  fileOperationV.reset();

  if ( Arinc615a::Tftp::TransferStatus::Successful != status )
  {
    SPDLOG_ERROR( "Transfer Error" );

    operationV->loadFinished(
      currentLoadV->headerFilename,
      Arinc615a::FinalStatus::LoadPartNumberOrDownloadFileFailed,
      "Transfer Error" );

    // update status
    operationV->inProgress( true, -1, {}, Arinc615a::Information::Ratio{ 0 } );

    ++currentLoadV;

    operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "File transfer Error" );

    return;
  }

  ++currentFileV;

  if ( currentFileV != filesV.end() )
  {
    // receive the next file
    boost::asio::post( ioContext(), std::bind_front( &TargetUploadOperation::receiveFile, this ) );
    return;
  }

  // complete current load
  operationV->loadInProgress( currentLoadV->headerFilename, Arinc615a::Information::Ratio{ 100 } );
  operationV->loadFinished( currentLoadV->headerFilename, Arinc615a::FinalStatus::Completed );
  operationV->inProgress( true, -1, {}, Arinc615a::Information::Ratio{ 0 } );

  // increment load iterators
  ++currentLoadV;

  if ( currentLoadV != loadsV.end() )
  {
    // receive next load
    boost::asio::post( ioContext(), std::bind_front( &TargetUploadOperation::receiveLoadHeader, this ) );
    return;
  }

  operationV->finished( Arinc615a::FinalStatus::Completed );
}

}
