// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Parity.
 **/

#include <helper/Parity.hpp>

#include <boost/test/unit_test.hpp>

#include <type_traits>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( ParityTest )

//! Odd parity test
BOOST_AUTO_TEST_CASE( parityOdd )
{
  auto calculateParityOdd = &calculateParity< uint32_t, false >;

  BOOST_CHECK( calculateParityOdd( 0x00000000UL ) == true );
  BOOST_CHECK( calculateParityOdd( 0x00000001UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000002UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000004UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000008UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000010UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000020UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000040UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000080UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000100UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000200UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000400UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00000800UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00001000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00002000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00004000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00008000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00010000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00020000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00040000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00080000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00100000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00200000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00400000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x00800000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x01000000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x02000000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x04000000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x08000000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x10000000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x20000000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x40000000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0x80000000UL ) == false );
  BOOST_CHECK( calculateParityOdd( 0xFFFFFFFFUL ) == true );

  BOOST_CHECK( calculateParityOdd( 0b0000'0000'0010'0100'1111'0000'0000'1000 ) == false );
  BOOST_CHECK( calculateParityOdd( 0b0100'0000'0010'0100'1111'0000'0000'1000 ) == true );
}

//! Even parity test
BOOST_AUTO_TEST_CASE( parityEven )
{
  auto calculateParityEven = &calculateParity< uint32_t, true >;

  BOOST_CHECK( calculateParityEven( 0x00000000UL ) == false );
  BOOST_CHECK( calculateParityEven( 0x00000001UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000002UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000004UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000008UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000010UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000020UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000040UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000080UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000100UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000200UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000400UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00000800UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00001000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00002000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00004000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00008000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00010000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00020000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00040000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00080000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00100000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00200000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00400000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x00800000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x01000000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x02000000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x04000000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x08000000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x10000000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x20000000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x40000000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0x80000000UL ) == true );
  BOOST_CHECK( calculateParityEven( 0xFFFFFFFFUL ) == false );

  BOOST_CHECK( calculateParityEven( 0b0000'0000'0010'0100'1111'0000'0000'1000 ) == true );
  BOOST_CHECK( calculateParityEven( 0b0100'0000'0010'0100'1111'0000'0000'1000 ) == false );
}

//! Odd Parity test
BOOST_AUTO_TEST_CASE( oddParity16 )
{
  auto calculateParityOdd = &calculateParity< uint16_t, false >;

  BOOST_CHECK( calculateParityOdd( 0x00000000UL ) == true );
  BOOST_CHECK( calculateParityOdd( 0b0000'0000'0000'0001 ) == false );
  BOOST_CHECK( calculateParityOdd( 0b1000'0000'0000'0001 ) == true );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
