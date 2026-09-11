// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Template Arinc649::HashCheckValueGenerator.
 **/

#include <arinc_649/CheckValue.hpp>

#include <helper/RawData.hpp>

namespace Arinc649 {

template< CheckValueType CVT, typename HashFunctionT >
void HashCheckValueGenerator< CVT, HashFunctionT >::reset()
{
  hashFunction = HashFunctionT{};
}

template< CheckValueType CVT, typename HashFunctionT >
void HashCheckValueGenerator< CVT, HashFunctionT >::process( Helper::ConstRawDataSpan data )
{
  hashFunction.update( data.data(), data.size() );
}

template< CheckValueType CVT, typename HashFunctionT >
CheckValue HashCheckValueGenerator< CVT, HashFunctionT >::checkValue()
{
  const auto hash{ hashFunction.result() };
  auto hashSpan{ Helper::RawData_asRawData( hash ) };
  return { CVT, { hashSpan.begin(), hashSpan.end() } };
}

}
