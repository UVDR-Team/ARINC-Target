// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc649 CheckValue.
 **/

#include "CheckValue.hpp"
#include <cstddef>

#include <arinc_649/CheckValueTypeDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <charconv>
#include <helper/Format.hpp>
#include <iostream>

namespace Arinc649 {

const CheckValue CheckValue::NoCheckValue{};

const std::map< CheckValueType, uint16_t >& CheckValue::checkValuesSize()
{
  static const std::map< CheckValueType, uint16_t> Sizes{
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

  return Sizes;
}

std::optional< uint16_t > CheckValue::crc16( const CheckValue &checkValue )
{
  const auto rawCrc{ checkValue.value() };

  if ( ( checkValue.type() != CheckValueType::Crc16 ) || ( rawCrc.size() != sizeof( uint16_t ) ) )
  {
    SPDLOG_ERROR( "No CRC 16 provided" );
    return std::nullopt;
  }

  auto [ _, crc16 ]{ Helper::RawData_getInt< uint16_t >( rawCrc ) };

  return crc16;
}

CheckValue CheckValue::crc16( const uint16_t crc )
{
  Helper::RawData checkValue( sizeof( uint16_t ) );
  Helper::RawData_setInt( checkValue, crc );
  return { CheckValueType::Crc16, std::move( checkValue ) };
}

CheckValue::CheckValue( const CheckValueType type, Helper::RawData value ) :
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
    SPDLOG_WARN( "String size is not a multiple of 2: {}", string.size() );
    return;
  }

  for ( auto pos{ std::size_t{0} }; pos != string.size(); pos +=2U )
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
    checkValueRawString += ARINC_FORMAT_NAMESPACE::format( "{:02X}", std::to_integer< uint8_t >( checkValueByte ) );
  }

  return checkValueRawString;
}

std::string CheckValue::format() const
{
  return ARINC_FORMAT_NAMESPACE::format( "{}:{}", CheckValueTypeDescription::instance().name( typeV ), toString() );
}

CheckValueType CheckValue::type() const
{
  return typeV;
}

Helper::ConstRawDataSpan CheckValue::value() const
{
  return valueV;
}

CheckValue::operator bool() const
{
  const auto &sizes{ checkValuesSize() };
  const auto size{ sizes.find( typeV ) };

  // check if the set check value type references a size
  if ( size == sizes.end() )
  {
    SPDLOG_WARN( "No size information for check value type" );
    return false;
  }

  // verify check value size
  if ( valueV.size() != size->second )
  {
    SPDLOG_WARN( "check value size invalid" );
    return false;
  }

  return true;
}

std::ostream& operator<<( std::ostream &stream, const CheckValue &checkValue )
{
  return stream << checkValue.format();
}

}
