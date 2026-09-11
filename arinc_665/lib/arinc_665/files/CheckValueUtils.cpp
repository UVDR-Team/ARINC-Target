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

#include <arinc_649/CheckValue.hpp>
#include <arinc_649/CheckValueTypeDescription.hpp>

#include <helper/Exception.hpp>
#include <helper/SafeCast.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

size_t CheckValueUtils_size( const Arinc649::CheckValueType type )
{
  return
    ( Arinc649::CheckValueType::NotUsed == type ) ?
      sizeof( uint16_t ) :
      ( 2U * sizeof( uint16_t ) ) + Arinc649::CheckValue::checkValuesSize().at( type );
}

Helper::RawData CheckValueUtils_encode( const Arinc649::CheckValue &checkValue )
{
  // special Handling of "No Check Value"
  if ( Arinc649::CheckValueType::NotUsed == checkValue.type() )
  {
    return { std::byte{ 0 }, std::byte{ 0 } };
  }

  // Length + Type
  Helper::RawData rawCheckValue( 2 * sizeof( uint16_t ) );

  // Check Value Type Field
  Helper::RawData_setInt< uint16_t >(
    Helper::RawDataSpan{ rawCheckValue }.subspan( sizeof( uint16_t ) ),
    static_cast< uint16_t >( checkValue.type() ) );

  // Check Value Data
  const auto &checkValueData{ checkValue.value() };

  rawCheckValue.insert( rawCheckValue.end(), checkValueData.begin(), checkValueData.end() );

  // Check Value Length
  Helper::RawData_setInt< uint16_t >( rawCheckValue, Helper::safeCast< uint16_t >( rawCheckValue.size() ) );

  return rawCheckValue;
}

Arinc649::CheckValue CheckValueUtils_decode( Helper::ConstRawDataSpan rawFile )
{
  // at least length field must be provided
  if ( rawFile.size() < sizeof( uint16_t ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "Invalid check value" } );
  }

  // Check Value Length
  auto [ remainingData, checkValueLength ] = Helper::RawData_getInt< uint16_t>( rawFile );

  // Special handling of empty check value
  if ( 0U == checkValueLength )
  {
    return Arinc649::CheckValue::NoCheckValue;
  }

  if ( checkValueLength < ( 2U * sizeof( uint16_t ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "Invalid length field of check value" } );
  }

  // Check Value Type
  uint16_t rawCheckValueType{};
  std::tie( remainingData, rawCheckValueType ) = Helper::RawData_getInt< uint16_t >( remainingData );

  const auto checkValueType{ Arinc649::CheckValueTypeDescription::instance().enumeration( rawCheckValueType ) };

  if ( !checkValueType )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "Invalid check value type" } );
  }

  // validate check value size
  if ( Arinc649::CheckValue::checkValuesSize().find( *checkValueType )->second != checkValueLength - ( 2U * sizeof( uint16_t ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "Invalid check value length" } );
  }

  remainingData = remainingData.first( checkValueLength - ( 2U * sizeof( uint16_t ) ) );

  return { *checkValueType, std::vector< std::byte >{ remainingData.begin(), remainingData.end() } };
}

}
