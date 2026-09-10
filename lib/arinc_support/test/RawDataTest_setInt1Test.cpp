// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for RawData_setInt Operations.
 **/

#include <arinc_support/RawData.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

namespace ArincSupport {

BOOST_AUTO_TEST_SUITE( HelperTest)
BOOST_AUTO_TEST_SUITE( RawData_setInt1Test )

//! Test Data
static constexpr uint8_t rawData[]{
  0x01, 0x23, 0x45, 0x67,
  0x89, 0xAB, 0xCD, 0xEF,
  0x0F, 0x1E, 0x2D, 0x3C,
  0x4B, 0x5A, 0x69, 0x78 };

//! Test Data as span
static const auto rawDataSpan{ RawData_asRawData( rawData ) };

//! Test String
static constexpr std::string_view string{ "TEST" };
//! Test String as raw Data
static const auto rawString{ RawData_asRawData( string ) };

//! setInt conversion test for uint8_t big endian
BOOST_AUTO_TEST_CASE( setInt8BigEndianTest )
{
  std::array< std::byte, 1 > data{};

  const auto retData{ RawData_setInt< uint8_t, boost::endian::order::big >( data, 0x01 ) } ;
  BOOST_CHECK( retData.empty() );
  BOOST_CHECK( std::ranges::equal( data, RawData_asRawData( std::to_array< uint8_t >( { 0x01 } ) ) ) );

  BOOST_CHECK_THROW( RawData_setInt< uint8_t >( ArincSupport::RawDataSpan{}, 0x01 ), std::out_of_range );
}

//! setInt conversion test for uint8_t little endian
BOOST_AUTO_TEST_CASE( setInt8LittleEndianTest )
{
  std::array< std::byte, 1 > data{};

  const auto retData{ RawData_setInt< uint8_t, boost::endian::order::little >( data, 0x01 ) } ;
  BOOST_CHECK( retData.empty() );
  BOOST_CHECK( std::ranges::equal( data, RawData_asRawData( std::to_array< uint8_t >( { 0x01 } ) ) ) );

  BOOST_CHECK_THROW( RawData_setInt< uint8_t >( ArincSupport::RawDataSpan{}, 0x01 ), std::out_of_range );
}

//! setInt conversion test for uint16_t big endian
BOOST_AUTO_TEST_CASE( setInt16BigEndianTest )
{
  std::array< std::byte, 2 > data{};

  const auto retData{ RawData_setInt< uint16_t, boost::endian::order::big >( data, 0x0123 ) } ;
  BOOST_CHECK( retData.empty() );
  BOOST_CHECK( std::ranges::equal( data, RawData_asRawData( std::to_array< uint8_t >( { 0x01, 0x23 } ) ) ) );

  BOOST_CHECK_THROW( RawData_setInt< uint16_t >( ArincSupport::RawDataSpan{}, 0x01 ), std::out_of_range );
}

//! setInt conversion test for uint16_t little endian
BOOST_AUTO_TEST_CASE( setInt16BigLittleTest )
{
  std::array< std::byte, 2 > data{};

  const auto retData{ RawData_setInt< uint16_t, boost::endian::order::little >( data, 0x0123 ) } ;
  BOOST_CHECK( retData.empty() );
  BOOST_CHECK( std::ranges::equal( data, RawData_asRawData( std::to_array< uint8_t >( { 0x23, 0x01 } ) ) ) );

  BOOST_CHECK_THROW( RawData_setInt< uint16_t >( ArincSupport::RawDataSpan{}, 0x01 ), std::out_of_range );
}

//! setInt conversion test for uint32_t big endian
BOOST_AUTO_TEST_CASE( setInt32BigEndianTest )
{
  std::array< std::byte, 4 > data{};

  const auto retData{ RawData_setInt< uint32_t, boost::endian::order::big >( data, 0x01234567 ) } ;
  BOOST_CHECK( retData.empty() );
  BOOST_CHECK(
    std::ranges::equal(
      data,
      RawData_asRawData( std::to_array< uint8_t >( { 0x01, 0x23, 0x45, 0x67 } ) ) ) );

  BOOST_CHECK_THROW( RawData_setInt< uint32_t >( ArincSupport::RawDataSpan{}, 0x01 ), std::out_of_range );
}

//! setInt conversion test for uint32_t little endian
BOOST_AUTO_TEST_CASE( setInt32LittleEndianTest )
{
  std::array< std::byte, 4 > data{};

  const auto retData{ RawData_setInt< uint32_t, boost::endian::order::little >( data, 0x0123'4567 ) } ;
  BOOST_CHECK( retData.empty() );
  BOOST_CHECK(
    std::ranges::equal(
      data,
      RawData_asRawData( std::to_array< uint8_t >( { 0x67, 0x45, 0x23, 0x01 } ) ) ) );

  BOOST_CHECK_THROW( RawData_setInt< uint32_t >( ArincSupport::RawDataSpan{}, 0x01 ), std::out_of_range );
}

//! setInt conversion test for uint64_t big endian
BOOST_AUTO_TEST_CASE( setInt64BigEndianTest )
{
  std::array< std::byte, 8 > data{};

  const auto retData{ RawData_setInt< uint64_t, boost::endian::order::big >( data, 0x0123'4567'89AB'CDEF ) } ;
  BOOST_CHECK( retData.empty() );
  BOOST_CHECK(
    std::ranges::equal(
      data,
      RawData_asRawData( std::to_array< uint8_t >( { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF } ) ) ) );

  BOOST_CHECK_THROW( RawData_setInt< uint64_t >( ArincSupport::RawDataSpan{}, 0x01 ), std::out_of_range );
}

//! setInt conversion test for uint64_t little endian
BOOST_AUTO_TEST_CASE( setInt64LittleEndianTest )
{
  std::array< std::byte, 8 > data{};

  const auto retData{ RawData_setInt< uint64_t, boost::endian::order::little >( data, 0x0123'4567'89AB'CDEF ) } ;
  BOOST_CHECK( retData.empty() );
  BOOST_CHECK(
    std::ranges::equal(
      data,
      RawData_asRawData( std::to_array< uint8_t >( { 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01 } ) ) ) );

  BOOST_CHECK_THROW( RawData_setInt< uint64_t >( ArincSupport::RawDataSpan{}, 0x01 ), std::out_of_range );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
