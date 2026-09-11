// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module NBitInteger.
 **/

#include <boost/exception/all.hpp>

#include <cassert>
#include <exception>
#include <limits>

namespace Helper {

template< std::unsigned_integral IntT >
constexpr IntT generateBitMask( const unsigned int minBit, const unsigned int maxBit )
{
  if ( minBit > maxBit )
  {
    BOOST_THROW_EXCEPTION( std::invalid_argument( "generateBitMask: minBit > maxBit" ) ) ;
  }

  if ( maxBit >= std::numeric_limits< IntT >::digits )
  {
    BOOST_THROW_EXCEPTION( std::invalid_argument( "generateBitMask: maxBit >= std::numeric_limits< IntT >::digits" ) ) ;
  };

  // Set upper mask (additional check to prevent overflow)
  const IntT upperMask{ ( maxBit == ( std::numeric_limits< IntT >::digits - 1 ) )
    ? std::numeric_limits< IntT >::max()
    : static_cast< IntT >( ( IntT{ 1U } << ( maxBit + 1U ) ) - IntT{ 1U } ) };

  // Set lower mask
  const IntT lowerMask{ static_cast< IntT >( ( IntT{ 1U } << minBit ) - 1U ) };

  // Combine upper mask with negated lower mask
  return ( upperMask & ~lowerMask );
}

template< std::unsigned_integral FullT, std::unsigned_integral SubT >
requires ( std::numeric_limits< FullT >::digits >= std::numeric_limits< SubT >::digits )
constexpr FullT subToFull( const SubT sub, const unsigned int minBit, const unsigned int maxBit, const FullT full )
{
  return ( full & ~generateBitMask< FullT >( minBit, maxBit ) )
    | ( ( static_cast< FullT >( sub & generateBitMask<SubT>( 0, ( maxBit - minBit ) ) ) ) << minBit );
}

template< std::unsigned_integral SubT, std::unsigned_integral FullT >
requires ( std::numeric_limits< FullT >::digits >= std::numeric_limits< SubT >::digits )
constexpr SubT fullToSub( const FullT full, const unsigned int minBit, const unsigned int maxBit )
{
  return static_cast< SubT >( ( full & generateBitMask< FullT >( minBit, maxBit ) ) >> minBit );
}

}
