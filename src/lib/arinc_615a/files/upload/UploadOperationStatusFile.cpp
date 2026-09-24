// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::UploadOperationStatusFile.
 **/

#include <cstddef>

#include <arinc_support/Support.hpp>

#include "UploadOperationStatusFile.hpp"

#include <arinc_615a/files/Ratio.hpp>
#include <arinc_615a/files/String.hpp>

#include <arinc_615a/Arinc615aException.hpp>
#include <arinc_615a/StatusCode.hpp>

#include <arinc_support/Exception.hpp>
#include <arinc_support/SafeCast.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/throw_exception.hpp>

#include <utility>

namespace Arinc615a::Files {

UploadOperationStatusFile::UploadOperationStatusFile(
  Arinc615aVersion const protocolVersion,
  Information::UploadStatus status ) :
  ProtocolFile{ protocolVersion },
  statusV{ std::move( status ) }
{
}

UploadOperationStatusFile::UploadOperationStatusFile( ArincSupport::ConstRawDataSpan rawData )
{
  decode( rawData );
}

UploadOperationStatusFile& UploadOperationStatusFile::operator=( ArincSupport::ConstRawDataSpan rawData )
{
  decode( rawData );
  return *this;
}

const Information::UploadStatus& UploadOperationStatusFile::status() const
{
  return statusV;
}

Information::UploadStatus& UploadOperationStatusFile::status()
{
  return statusV;
}

void UploadOperationStatusFile::status( Information::UploadStatus status )
{
  statusV = std::move( status );
}

ArincSupport::RawData UploadOperationStatusFile::encode() const
{
  // Reserver Size for header and status code
  ArincSupport::RawData rawData( HeaderSize + sizeof( uint16_t ) );

  // skip header - it is filled finally
  auto nextData{ ArincSupport::RawDataSpan{ rawData }.subspan( HeaderSize ) };

  // status code
  ArincSupport::RawData_setInt( nextData, ArincSupport::toUnderlying( statusV.code() ) );

  // status description
  const auto rawDescription{ String_encode( statusV.description() ) };
  rawData.insert( rawData.end(), rawDescription.begin(), rawDescription.end() );

  // reserve and resize buffer for status counter, exception timer, and estimated time
  rawData.resize( rawData.size() + ( std::size_t{3} * sizeof( uint16_t ) ) );
  nextData = ArincSupport::RawDataSpan{ rawData }.last( std::size_t{3} * sizeof( uint16_t ) );

  // counter
  nextData = ArincSupport::RawData_setInt( nextData, statusV.counter() );

  // exception timer
  nextData = ArincSupport::RawData_setInt( nextData, statusV.exceptionTimer() );

  // estimated time
  nextData = ArincSupport::RawData_setInt( nextData, statusV.estimatedTime() );
  assert( nextData.empty() );

  // load list ratio
  const auto loadListRatio{ Ratio_encode( statusV.listRatio() ) };
  rawData.insert( rawData.end(), loadListRatio.begin(), loadListRatio.end() );

  // resize buffer for number of header files
  rawData.resize( rawData.size() + sizeof( uint16_t ) );
  nextData = ArincSupport::RawDataSpan{ rawData }.last( sizeof( uint16_t ) );

  // the number of loads must not exceed the field max value
  if ( statusV.loads().size() > std::numeric_limits< uint16_t >::max() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{}
      << ArincSupport::AdditionalInfo{ "More Load Headers than allowed" } );
  }

  // number of header files
  ArincSupport::RawData_setInt( nextData, ArincSupport::safeCast< uint16_t >( statusV.loads().size() ) );

  // add each header file status
  for ( const auto &headerFile : statusV.loads() )
  {
    // Header filename
    const auto rawHeaderFilename{ String_encode( headerFile.headerFilename() ) };
    rawData.insert( rawData.end(), rawHeaderFilename.begin(), rawHeaderFilename.end() );

    // Load part number
    const auto rawLoadPartNumber{ String_encode( headerFile.partNumber() ) };
    rawData.insert( rawData.end(), rawLoadPartNumber.begin(), rawLoadPartNumber.end() );

    // load ratio
    const auto loadRatio{ Ratio_encode( headerFile.ratio() ) };
    rawData.insert( rawData.end(), loadRatio.begin(), loadRatio.end() );

    // resize buffer for load status code
    rawData.resize( rawData.size() + sizeof( uint16_t ) );
    nextData = ArincSupport::RawDataSpan{ rawData }.last( sizeof( uint16_t ) );

    // load status code
    nextData = ArincSupport::RawData_setInt( nextData, ArincSupport::toUnderlying( headerFile.code() ) );
    assert( nextData.empty() );

    // Load status description
    const auto rawLoadDescription{ String_encode( headerFile.description() ) };
    rawData.insert( rawData.end(), rawLoadDescription.begin(), rawLoadDescription.end() );
  }

  // insert header
  insertHeader( rawData );

  return rawData;
}

void UploadOperationStatusFile::decode( ArincSupport::ConstRawDataSpan rawData )
{
  // check minimum data size
  if ( rawData.size() < ( HeaderSize + std::size_t{9} ) )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << ArincSupport::AdditionalInfo{ "Protocol file to small" } );
  }

  auto remainingData{ decodeHeader( rawData ) };

  // status code
  uint16_t intStatusCode;
  std::tie( remainingData, intStatusCode ) = ArincSupport::RawData_getInt< uint16_t >( remainingData );
  statusV.code( statusCode( intStatusCode ) );

  // status description
  std::string statusDescription;
  std::tie( remainingData, statusDescription ) = String_decode( remainingData );
  statusV.description( std::move( statusDescription ) );

  // counter
  uint16_t counter;
  std::tie( remainingData, counter ) = ArincSupport::RawData_getInt< uint16_t >( remainingData );
  statusV.counter( counter );

  // exception timer
  uint16_t exceptionTimer;
  std::tie( remainingData, exceptionTimer ) = ArincSupport::RawData_getInt< uint16_t >( remainingData );
  statusV.exceptionTimer( exceptionTimer );

  // estimated time
  int16_t estimatedTime;
  std::tie( remainingData, estimatedTime ) = ArincSupport::RawData_getInt< int16_t >( remainingData );
  statusV.estimatedTime( estimatedTime );

  // load list ratio
  Information::Ratio loadListRatio;
  std::tie( remainingData, loadListRatio) = Ratio_decode( remainingData );
  statusV.listRatio( loadListRatio );

  // number of header files
  uint16_t numberOfHeaderFiles;
  std::tie( remainingData, numberOfHeaderFiles ) = ArincSupport::RawData_getInt< uint16_t >( remainingData );

  Information::UploadLoadsStatus loadsStatus;

  // iterate over header files
  for ( uint16_t headerFileIndex{ 0U }; headerFileIndex < numberOfHeaderFiles; ++headerFileIndex )
  {
    try
    {
      // header filename
      std::string headerFilename;
      std::tie( remainingData, headerFilename ) = String_decode( remainingData );
      if ( headerFilename.empty() )
      {
        ARINC_LOG_WARN( "header filename is empty" );
      }

      // load part number
      std::string loadPartNumber;
      std::tie( remainingData, loadPartNumber ) = String_decode( remainingData );
      if ( loadPartNumber.empty() )
      {
        ARINC_LOG_WARN( "load part number is empty" );
      }

      // load ratio
      Information::Ratio loadRatio;
      std::tie( remainingData, loadRatio ) = Ratio_decode( remainingData );

      // load status code
      uint16_t intLoadStatusCode;
      std::tie( remainingData, intLoadStatusCode ) = ArincSupport::RawData_getInt< uint16_t >( remainingData );

      // load status description
      std::string loadStatusDescription;
      std::tie( remainingData, loadStatusDescription ) = String_decode( remainingData );

      loadsStatus.emplace_back(
        std::move( headerFilename ),
        std::move( loadPartNumber ),
        std::move( loadRatio ),
        statusCode( intLoadStatusCode ),
        std::move( loadStatusDescription ) );
    }
    catch ( const std::invalid_argument & )
    {
      BOOST_THROW_EXCEPTION( Arinc615aException{} << ArincSupport::AdditionalInfo{ "Decoding Load Ratio" } );
    }
  }

  statusV.loads( std::move( loadsStatus ) );

  // Final Check for additional data
  if ( !remainingData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << ArincSupport::AdditionalInfo{ "More data then expected" } );
  }
}

}
