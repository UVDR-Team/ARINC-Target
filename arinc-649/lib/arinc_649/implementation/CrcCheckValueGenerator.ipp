// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc649::CrcCheckValueGenerator.
 **/

#include <helper/RawData.hpp>

namespace Arinc649 {

template< typename CrcGeneratorT, typename CrcValueT, CheckValueType CheckValueTypeV >
void CrcCheckValueGenerator< CrcGeneratorT, CrcValueT, CheckValueTypeV >::reset()
{
  crcGenerator.reset();
}

template< typename CrcGeneratorT, typename CrcValueT, CheckValueType CheckValueTypeV >
void CrcCheckValueGenerator< CrcGeneratorT, CrcValueT, CheckValueTypeV >::process( Helper::ConstRawDataSpan data )
{
  crcGenerator.process_bytes( std::data( data ), data.size() );
}

template< typename CrcGeneratorT, typename CrcValueT, CheckValueType CheckValueTypeV >
CheckValue CrcCheckValueGenerator< CrcGeneratorT, CrcValueT, CheckValueTypeV >::checkValue()
{
  if constexpr ( std::is_same_v< CrcGenerator, Arinc649Crc8 > )
  {
    // CRC 8 is extended to 16 bit
    return { Type, Helper::RawData{ std::byte{ 0U }, std::byte{ crcGenerator.checksum() } } };
  }
  else
  {
    Helper::RawData checkValue( sizeof( CrcValueType ) );
    Helper::RawData_setInt< CrcValueType >( checkValue, crcGenerator.checksum() );

    return { Type, std::move( checkValue ) };
  }
}

}
