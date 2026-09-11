// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Helper::NBitInteger.
 **/

#include <helper/NBitInteger.hpp>

#include <boost/test/unit_test.hpp>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest)
BOOST_AUTO_TEST_SUITE( NBitIntegerTest )

//! Walking 1 Bit test
BOOST_AUTO_TEST_CASE( generateBitMask_walking1 )
{
  for ( unsigned int bitNum = 0; bitNum < 32; ++bitNum )
  {
    BOOST_CHECK( generateBitMask< uint32_t>( bitNum, bitNum ) == 1UL << bitNum );
  }
}

//! Pattern Bit test
BOOST_AUTO_TEST_CASE( generateBitMask_patterns )
{
  BOOST_CHECK( generateBitMask< uint32_t >( 4, 27 ) == 0b0000'1111'1111'1111'1111'1111'1111'0000 );
  BOOST_CHECK( generateBitMask< uint32_t >( 11, 20 ) == 0b0000'0000'0001'1111'1111'1000'0000'0000 );
  BOOST_CHECK( generateBitMask< uint32_t >( 31, 31 ) == 0b1000'0000'0000'0000'0000'0000'0000'0000 );
}

//! Sub pattern bit tests
BOOST_AUTO_TEST_CASE( subToFull_patterns )
{
  BOOST_CHECK( ( subToFull< uint64_t, uint8_t >( 0xAA, 8, 15, 0 ) == 0x0000'0000'0000'AA00ULL ) );
  BOOST_CHECK( ( subToFull< uint64_t, uint8_t >( 0x55, 8, 15, 0xFFFF'FFFF'FFFF'FFFF ) == 0xFFFF'FFFF'FFFF'55FFULL ) );
}

//! Sub pattern bit tests
BOOST_AUTO_TEST_CASE( fullToSub_patterns )
{
  constexpr auto value1{ fullToSub< uint8_t, uint64_t >( 0x0000'0001'0000'0000ULL, 32, 32 ) };
  BOOST_CHECK( value1 == 1 );

  constexpr auto value2{ fullToSub< uint8_t, uint64_t >( 0x0000'0000'8000'0000ULL, 31, 31 ) };
  BOOST_CHECK( value2 == 1 );

  constexpr auto value3{ fullToSub< uint8_t, uint64_t >( 0x0000'0000'4000'0000ULL, 30, 30 ) };
  BOOST_CHECK( value3 == 1 );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
