// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Safe Cast.
 **/

#include <helper/SafeCast.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( SafeCastTest )

//! Safe Cast test
BOOST_AUTO_TEST_CASE( test1 )
{
  BOOST_CHECK( ( safeCast< uint16_t, uint32_t >( 0 ) ) == 0 );
  BOOST_CHECK( ( safeCast< uint16_t, uint32_t >( 1 ) ) == 1 );
  BOOST_CHECK( ( safeCast< uint16_t, uint32_t >( 0xFFFEUL ) ) == 0xFFFEUL );
  BOOST_CHECK( ( safeCast< uint16_t, uint32_t >( 0xFFFFUL ) ) == 0xFFFFUL );

  BOOST_CHECK_THROW(
    boost::ignore_unused( safeCast< uint16_t, uint32_t >( 0x1'0000UL ) ),
    std::range_error );
  BOOST_CHECK_THROW( boost::ignore_unused( safeCast<uint16_t, uint32_t>( 0xFFFF'FFFFUL ) ), std::range_error );
}

//! Safe Cast test
BOOST_AUTO_TEST_CASE( test2 )
{
  BOOST_CHECK( ( safeCast< uint16_t, int32_t >( 0 ) ) == 0 );
  BOOST_CHECK( ( safeCast< uint16_t, int32_t >( 1 ) ) == 1 );
  BOOST_CHECK( ( safeCast< uint16_t, int32_t >( 0xFFFEUL ) ) == 0xFFFEUL );
  BOOST_CHECK( ( safeCast< uint16_t, int32_t >( 0xFFFFUL ) ) == 0xFFFFUL );

  BOOST_CHECK_THROW( boost::ignore_unused( safeCast< uint16_t, int32_t >( 0x1'0000L ) ), std::range_error );
  BOOST_CHECK_THROW( boost::ignore_unused( safeCast< uint16_t, int32_t >( 0x7FFF'FFFFL ) ), std::range_error );
  BOOST_CHECK_THROW( boost::ignore_unused( safeCast< uint16_t, int32_t >( -1 ) ), std::range_error );
}

//! in range
BOOST_AUTO_TEST_CASE( inRangeTest )
{
  BOOST_CHECK( inRange< uint8_t >( 0 ) );
  BOOST_CHECK( inRange< uint8_t >( 255 ) );
  BOOST_CHECK( !inRange< uint8_t >( 256 ) );

  BOOST_CHECK( ( !inRange< uint8_t, 1, 254 >( 0 ) ) );
  BOOST_CHECK( ( inRange< uint8_t, 1, 254 >( 1 ) ) );
  BOOST_CHECK( ( inRange< uint8_t, 1, 254 >( 254 ) ) );
  BOOST_CHECK( ( !inRange< uint8_t, 1 ,254 >( 255 ) ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
