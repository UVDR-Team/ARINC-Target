// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of BCD Conversion Functions.
 **/

#include <boost/exception/all.hpp>

#include <stdexcept>

namespace Helper {

template< std::unsigned_integral OutT, std::unsigned_integral InT >
constexpr OutT toBcd( const InT value )
{
  static_assert( sizeof( OutT ) >= sizeof( InT ) );

  OutT result{ 0 };

  InT convertValue{ value };
  unsigned int shift{ 0 };

  while ( 0U != convertValue )
  {
    OutT digit{ (convertValue % 10) << shift };
    convertValue /= 10;
    shift += 4;
    result |= digit;
  }

  return result;
}

template< std::unsigned_integral OutT, std::unsigned_integral InT >
constexpr OutT fromBcd( const InT value )
{
  OutT result{ 0 };

  InT convertValue{ value };
  OutT factor{ 1U };

  while ( 0U != convertValue )
  {
    const OutT digit{ convertValue & 0xFU };
    if ( digit > 9U )
    {
      BOOST_THROW_EXCEPTION( std::range_error{ "digit not between 0..9" } );
    }
    result += digit * factor;
    convertValue >>= 4U;
    factor *= 10;
  }

  return result;
}

}
