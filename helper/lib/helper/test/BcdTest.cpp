// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for BCD Operations.
 **/

#include <helper/Bcd.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( BcdTest )

//! To BCD conversion test
BOOST_AUTO_TEST_CASE( toBcdTest )
{
  BOOST_CHECK( toBcd<uint32_t>( 0U ) == 0x0U );
  BOOST_CHECK( toBcd<uint32_t>( 1234U ) == 0x1234U );
  BOOST_CHECK( toBcd<uint32_t>( 7654'3210U ) == 0x7654'3210U );

  BOOST_CHECK( toBcd<uint64_t>( 9876'5432'1001'2345U ) == 0x9876'5432'1001'2345U );
}

//! From BCD conversion test
BOOST_AUTO_TEST_CASE( fromBcdTest )
{
  BOOST_CHECK( fromBcd<uint32_t>( 0x0U ) == 0U );
  BOOST_CHECK( fromBcd<uint32_t>( 0x1234U ) == 1234U );
  BOOST_CHECK( fromBcd<uint32_t>( 0x7654'3210U ) == 7654'3210U );

  BOOST_CHECK( fromBcd<uint64_t>( 0x9876'5432'1001'2345U ) == 9876'5432'1001'2345U );

  BOOST_CHECK_THROW(
    boost::ignore_unused( fromBcd< uint32_t >( 0x01A3'4567U ) ),
    std::range_error );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
