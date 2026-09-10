// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc645 CheckValue.
 **/

#include "CheckValue.hpp"

#include <arinc_checksum/CheckValueTypeDescription.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/exception/all.hpp>

#include <charconv>
#include <format>
#include <iostream>

namespace ArincChecksum {

const std::map< CheckValueType, uint16_t> CheckValue::Sizes{
  { CheckValueType::NotUsed, 0U },

  { CheckValueType::Crc8,    2U },
  { CheckValueType::Crc16,   2U },
  { CheckValueType::Crc32,   4U },
  { CheckValueType::Md5,    16U },
  { CheckValueType::Sha1,   20U },
  { CheckValueType::Sha256, 32U },
  { CheckValueType::Sha512, 64U },
  { CheckValueType::Crc64,   8U }
};

const CheckValue CheckValue::NoCheckValue{};

std::optional< uint16_t > CheckValue::crc16( const CheckValue &checkValue )
{
  const auto rawCrc{ checkValue.value() };

  if ( ( checkValue.type() != CheckValueType::Crc16 ) || ( rawCrc.size() != sizeof( uint16_t ) ) )
  {
    ARINC_LOG_ERROR( "No CRC 16 provided" );
    return std::nullopt;
  }

  auto [ _, crc16 ]{ ArincSupport::RawData_getInt< uint16_t >( rawCrc ) };

  return crc16;
}

CheckValue CheckValue::crc16( const uint16_t crc )
{
  ArincSupport::RawData checkValue( sizeof( uint16_t ) );
  ArincSupport::RawData_setInt( checkValue, crc );
  return { CheckValueType::Crc16, std::move( checkValue ) };
}

CheckValue::CheckValue( const CheckValueType type, ArincSupport::RawData value ) :
  typeV{ type },
  valueV{ std::move( value ) }
{
}

CheckValue::CheckValue( const CheckValueType type, std::string_view string ) :
  typeV{ type }
{
  // check size modulo
  if ( string.size() % 2U != 0U )
  {
    ARINC_LOG_WARN( "String size is not a multiple of 2: {}", string.size() );
    return;
  }

  for ( auto pos{ 0UZ }; pos != string.size(); pos +=2U )
  {
    uint8_t value{};
    std::from_chars( string.data() + pos, string.data() + pos + 2, value, 16 );
    valueV.emplace_back( std::byte{ value } );
  }
}

std::string CheckValue::toString() const
{
  std::string checkValueRawString;
  checkValueRawString.reserve( valueV.size() * 2U );

  // check value string
  for ( const auto &checkValueByte : valueV )
  {
    checkValueRawString += std::format( "{:02X}", std::to_integer< uint8_t >( checkValueByte ) );
  }

  return checkValueRawString;
}

std::string CheckValue::format() const
{
  return std::format( "{}:{}", CheckValueTypeDescription::instance().name( typeV ), toString() );
}

CheckValueType CheckValue::type() const
{
  return typeV;
}

ArincSupport::ConstRawDataSpan CheckValue::value() const
{
  return valueV;
}

CheckValue::operator bool() const
{
  const auto size{ Sizes.find( typeV ) };

  // check if the set check value type references a size
  if ( size == Sizes.end() )
  {
    ARINC_LOG_WARN( "No size information for check value type" );
    return false;
  }

  // verify check value size
  if ( valueV.size() != size->second )
  {
    ARINC_LOG_WARN( "check value size invalid" );
    return false;
  }

  return true;
}

std::ostream& operator<<( std::ostream &stream, const CheckValue &checkValue )
{
  return stream << checkValue.format();
}

}
