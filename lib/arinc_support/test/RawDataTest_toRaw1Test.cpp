// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Raw Data Operations.
 **/

#include <arinc_support/RawData.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <array>
#include <cstdint>

namespace ArincSupport {

BOOST_AUTO_TEST_SUITE( HelperTest)
BOOST_AUTO_TEST_SUITE( RawData_toRaw1Test )

//! Test String
static constexpr std::string_view string{ "TEST" };
//! Test String as raw Data
static const auto rawString{ RawData_asRawData( string ) };

//! RawData_toRaw conversion test for uint8_t big endian
BOOST_AUTO_TEST_CASE( toRawInt8BigEndianTest )
{
  auto data{ RawData_toRaw< uint8_t, boost::endian::order::big >( 0x01 ) } ;
  BOOST_CHECK( data.size() == 1 );
  BOOST_CHECK( std::ranges::equal( data, RawData_asRawData( std::to_array< uint8_t >( { 0x01 } ) ) ) );
}

//! RawData_toRaw conversion test for uint8_t little endian
BOOST_AUTO_TEST_CASE( toRawInt8LittleEndianTest )
{
  auto data{ RawData_toRaw< uint8_t, boost::endian::order::little >( 0x01 ) } ;
  BOOST_CHECK( data.size() == 1 );
  BOOST_CHECK( std::ranges::equal( data, RawData_asRawData( std::to_array< uint8_t >( { 0x01 } ) ) ) );
}

//! RawData_toRaw conversion test for uint16_t big endian
BOOST_AUTO_TEST_CASE( toRawInt16BigEndianTest )
{
  auto data{ RawData_toRaw< uint16_t, boost::endian::order::big >( 0x0123 ) } ;
  BOOST_CHECK( data.size() == 2 );
  BOOST_CHECK( std::ranges::equal( data, RawData_asRawData( std::to_array< uint8_t >( { 0x01, 0x23 } ) ) ) );
}

//! RawData_toRaw conversion test for uint16_t little endian
BOOST_AUTO_TEST_CASE( toRawInt16LittleEndianTest )
{
  auto data{ RawData_toRaw< uint16_t, boost::endian::order::little >( 0x0123 ) } ;
  BOOST_CHECK( data.size() == 2 );
  BOOST_CHECK( std::ranges::equal( data, RawData_asRawData( std::to_array< uint8_t >( { 0x23, 0x01 } ) ) ) );
}

//! RawData_toRaw conversion test for uint32_t big endian
BOOST_AUTO_TEST_CASE( toRawInt32BigEndianTest )
{
  auto data{ RawData_toRaw< uint32_t, boost::endian::order::big >( 0x0123'4567 ) } ;
  BOOST_CHECK( data.size() == 4 );
  BOOST_CHECK(
    std::ranges::equal(
      data,
      RawData_asRawData( std::to_array< uint8_t >( { 0x01, 0x23, 0x45, 0x67 } ) ) ) );
}

//! RawData_toRaw conversion test for uint32_t little endian
BOOST_AUTO_TEST_CASE( toRawInt32BigLittleTest )
{
  auto data{ RawData_toRaw< uint32_t, boost::endian::order::little >( 0x0123'4567 ) } ;
  BOOST_CHECK( data.size() == 4 );
  BOOST_CHECK(
    std::ranges::equal(
      data,
      RawData_asRawData( std::to_array< uint8_t >( { 0x67, 0x45, 0x23, 0x01 } ) ) ) );
}

//! RawData_toRaw conversion test for uint64_t big endian
BOOST_AUTO_TEST_CASE( toRawInt64BigEndianTest )
{
  auto data{ RawData_toRaw< uint64_t, boost::endian::order::big >( 0x0123'4567'89AB'CDEF ) } ;
  BOOST_CHECK( data.size() == 8 );
  BOOST_CHECK(
    std::ranges::equal(
      data,
      RawData_asRawData( std::to_array< uint8_t >( { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF } ) ) ) );
}

//! RawData_toRaw conversion test for uint64_t little endian
BOOST_AUTO_TEST_CASE( toRawInt64LittleEndianTest )
{
  auto data{ RawData_toRaw< uint64_t, boost::endian::order::little >( 0x0123'4567'89AB'CDEF ) } ;
  BOOST_CHECK( data.size() == 8 );
  BOOST_CHECK(
    std::ranges::equal(
      data,
      RawData_asRawData( std::to_array< uint8_t >( { 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01 } ) ) ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
