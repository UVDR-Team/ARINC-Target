// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::StatusCode.
 **/

#include "StatusCode.hpp"

#include <arinc_615a/Arinc615aException.hpp>

#include <arinc_support/Exception.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/exception/all.hpp>

#include <arinc_support/Format.hpp>

namespace Arinc615a {

StatusCode statusCode( const FinalStatus status )
{
  switch ( status )
  {
    case FinalStatus::Completed:
      return StatusCode::OperationCompleted;

    case FinalStatus::AbortedByDlp:
      return StatusCode::OperationAbortedByDlp;

    case FinalStatus::AbortedByOperator:
      return StatusCode::OperationAbortedByOperator;

    case FinalStatus::AbortedByTargetHardware:
      return StatusCode::OperationAbortedByTargetHw;

    default:
      ARINC_LOG_WARN( "Invalid final status" );
      return StatusCode::OperationAbortedByTargetHw;
  }
}

FinalStatus finalStatus( const StatusCode code )
{
  switch ( code )
  {
    case StatusCode::OperationCompleted:
      return FinalStatus::Completed;

    case StatusCode::OperationAbortedByDlp:
      return FinalStatus::AbortedByDlp;

    case StatusCode::OperationAbortedByOperator:
      return FinalStatus::AbortedByOperator;

    case StatusCode::OperationAbortedByTargetHw:
      return FinalStatus::AbortedByTargetHardware;

    case StatusCode::LoadPartNumberOrDownloadFileFailed:
      return FinalStatus::LoadPartNumberOrDownloadFileFailed;

    default:
      ARINC_LOG_WARN( "Invalid status code" );
      return FinalStatus::AbortedByTargetHardware;
  }
}

FinalStatus finalStatus( const AbortRequest abortRequest )
{
  switch ( abortRequest )
  {
    case AbortRequest::AbortByDlp:
      return Arinc615a::FinalStatus::AbortedByDlp;

    case AbortRequest::AbortByOperator:
      return Arinc615a::FinalStatus::AbortedByOperator;

    default:
      ARINC_LOG_WARN( "Invalid abort request" );
      return Arinc615a::FinalStatus::AbortedByTargetHardware;
  }
}

StatusCode statusCode( uint16_t const code )
{
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  switch ( StatusCode{ code } )
  {
    case StatusCode::OperationAccepted:
    case StatusCode::OperationNotAccepted:
    case StatusCode::OperationNotSupported:
    case StatusCode::OperationInProgress:
    case StatusCode::OperationCompleted:
    case StatusCode::OperationInProgressAdditionalInfo:
    case StatusCode::OperationAbortedByTargetHw:
    case StatusCode::OperationAbortedByDlp:
    case StatusCode::OperationAbortedByOperator:
    case StatusCode::LoadPartNumberOrDownloadFileFailed:
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc615aException{} << ArincSupport::AdditionalInfo{ "Invalid status code" } );
  }

  return StatusCode{ code };
}

OperationAcceptanceStatusCode operationAcceptanceStatusCode( uint16_t const code )
{
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  switch ( OperationAcceptanceStatusCode{ code } )
  {
    case OperationAcceptanceStatusCode::OperationAccepted:
    case OperationAcceptanceStatusCode::OperationDenied:
    case OperationAcceptanceStatusCode::OperationNotSupported:
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc615aException{} << ArincSupport::AdditionalInfo{ "Invalid status code" } );
  }

  return OperationAcceptanceStatusCode{ code };
}

std::string status(
  OperationClass const operationClass,
  StatusCode const code,
  std::string_view const description,
  std::string_view const loadPartNumberOrFilename )
{
  std::string_view operation;

  switch ( operationClass )
  {
    case OperationClass::Information:
      operation = "Information";
      break;

    case OperationClass::Upload:
      operation = "Uploading";
      break;

    case OperationClass::Download:
      operation = "Downloading";
      break;

    default:
      return "**INVALID**";
  }

  switch ( code )
  {
    case StatusCode::OperationAccepted:
      return {};

    case StatusCode::OperationNotAccepted:
      return ArincSupport::format( "{} Operation Denied. {}", operation, description );

    case StatusCode::OperationNotSupported:
      return ArincSupport::format( "{} Operation not supported by the target. {}", operation, description );

    case StatusCode::OperationInProgress:
      return {};

    case StatusCode::OperationCompleted:
      return ArincSupport::format( "{} Operation Completed.", operation );

    case StatusCode::OperationInProgressAdditionalInfo:
      return std::string{ description };

    case StatusCode::OperationAbortedByTargetHw:
      return ArincSupport::format( "{} Operation aborted by the Target Hardware. {}", operation, description );

    case StatusCode::OperationAbortedByDlp:
      return ArincSupport::format( "{} Operation aborted by the Data Loader. {}", operation, description );

    case StatusCode::OperationAbortedByOperator:
      return ArincSupport::format( "{} Operation cancelled by the operator.", operation );

    case StatusCode::LoadPartNumberOrDownloadFileFailed:
      return ArincSupport::format( "{} failed. {}", loadPartNumberOrFilename, description );

    case StatusCode::OperationDeferred:
      return ArincSupport::format( "{} Operation deferred. {}", operation, description );

    default:
      return "**INVALID**";
  }
}

std::string status(
  OperationClass const operationClass,
  OperationAcceptanceStatusCode const code,
  std::string_view const description )
{
  return status( operationClass, static_cast< StatusCode >( code ), description );
}

}
