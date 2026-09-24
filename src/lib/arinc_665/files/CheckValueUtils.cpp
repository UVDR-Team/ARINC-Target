// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Check Value Utility Functions.
 **/

#include "CheckValueUtils.hpp"

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_checksum/CheckValue.hpp>
#include <arinc_checksum/CheckValueTypeDescription.hpp>

#include <arinc_support/Exception.hpp>
#include <arinc_support/SafeCast.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

size_t CheckValueUtils_size( const ArincChecksum::CheckValueType type )
{
  return
    ( ArincChecksum::CheckValueType::NotUsed == type ) ?
      sizeof( uint16_t ) :
      ( 2U * sizeof( uint16_t ) ) + ArincChecksum::CheckValue::Sizes.at( type );
}

ArincSupport::RawData CheckValueUtils_encode( const ArincChecksum::CheckValue &checkValue )
{
  // special Handling of "No Check Value"
  if ( ArincChecksum::CheckValueType::NotUsed == checkValue.type() )
  {
    return { std::byte{ 0 }, std::byte{ 0 } };
  }

  // Length + Type
  ArincSupport::RawData rawCheckValue( 2 * sizeof( uint16_t ) );

  // Check Value Type Field
  ArincSupport::RawData_setInt< uint16_t >(
    ArincSupport::RawDataSpan{ rawCheckValue }.subspan( sizeof( uint16_t ) ),
    static_cast< uint16_t >( checkValue.type() ) );

  // Check Value Data
  const auto &checkValueData{ checkValue.value() };

  rawCheckValue.insert( rawCheckValue.end(), checkValueData.begin(), checkValueData.end() );

  // Check Value Length
  ArincSupport::RawData_setInt< uint16_t >( rawCheckValue, ArincSupport::safeCast< uint16_t >( rawCheckValue.size() ) );

  return rawCheckValue;
}

ArincChecksum::CheckValue CheckValueUtils_decode( ArincSupport::ConstRawDataSpan rawFile )
{
  // at least length field must be provided
  if ( rawFile.size() < sizeof( uint16_t ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << ArincSupport::AdditionalInfo{ "Invalid check value" } );
  }

  // Check Value Length
  auto [ remainingData, checkValueLength ] = ArincSupport::RawData_getInt< uint16_t>( rawFile );

  // Special handling of empty check value
  if ( 0U == checkValueLength )
  {
    return ArincChecksum::CheckValue::NoCheckValue;
  }

  if ( checkValueLength < ( 2U * sizeof( uint16_t ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << ArincSupport::AdditionalInfo{ "Invalid length field of check value" } );
  }

  // Check Value Type
  uint16_t rawCheckValueType{};
  std::tie( remainingData, rawCheckValueType ) = ArincSupport::RawData_getInt< uint16_t >( remainingData );

  const auto checkValueType{ ArincChecksum::CheckValueTypeDescription::instance().enumeration( rawCheckValueType ) };

  if ( !checkValueType )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << ArincSupport::AdditionalInfo{ "Invalid check value type" } );
  }

  // validate check value size
  if ( ArincChecksum::CheckValue::Sizes.find( *checkValueType )->second != checkValueLength - ( 2U * sizeof( uint16_t ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << ArincSupport::AdditionalInfo{ "Invalid check value length" } );
  }

  remainingData = remainingData.first( checkValueLength - ( 2U * sizeof( uint16_t ) ) );

  return { *checkValueType, std::vector< std::byte >{ remainingData.begin(), remainingData.end() } };
}

}
