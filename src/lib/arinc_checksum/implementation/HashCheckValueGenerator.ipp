// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Template ArincChecksum::HashCheckValueGenerator.
 **/

#include <arinc_checksum/CheckValue.hpp>

#include <arinc_support/RawData.hpp>

namespace ArincChecksum {

template< CheckValueType CVT, typename HashFunctionT >
void HashCheckValueGenerator< CVT, HashFunctionT >::reset()
{
  hashFunction = HashFunctionT{};
}

template< CheckValueType CVT, typename HashFunctionT >
void HashCheckValueGenerator< CVT, HashFunctionT >::process( ArincSupport::ConstRawDataSpan data )
{
  hashFunction.update( data.data(), data.size() );
}

template< CheckValueType CVT, typename HashFunctionT >
CheckValue HashCheckValueGenerator< CVT, HashFunctionT >::checkValue()
{
  const auto hash{ hashFunction.result() };
  auto hashSpan{ ArincSupport::RawData_asRawData( hash ) };
  return { CVT, { hashSpan.begin(), hashSpan.end() } };
}

}
