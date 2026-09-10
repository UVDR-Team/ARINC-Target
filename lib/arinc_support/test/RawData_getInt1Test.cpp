// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for ArincSupport::RawData_getInt Operations.
 **/

#include <arinc_support/RawData.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <vector>

namespace ArincSupport {

BOOST_AUTO_TEST_SUITE( HelperTest)
BOOST_AUTO_TEST_SUITE( RawData_getInt1Test )

//! Test Data
static constexpr uint8_t rawData[]{
  0x01, 0x23, 0x45, 0x67,
  0x89, 0xAB, 0xCD, 0xEF,
  0x0F, 0x1E, 0x2D, 0x3C,
  0x4B, 0x5A, 0x69, 0x78 };

//! Test Data as span
static const auto rawDataSpan{ RawData_asRawData( rawData ) };

//! getInt conversion test for uint8_t big endian
BOOST_AUTO_TEST_CASE( getInt8BigEndianTest )
{
  ConstRawDataSpan data{ rawDataSpan };

  uint8_t value{};
  std::tie( data, value ) = RawData_getInt< uint8_t, boost::endian::order::big > ( data );
  BOOST_CHECK( value == 0x01 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 1 );

  std::tie( data, value ) = RawData_getInt< uint8_t, boost::endian::order::big > ( data );
  BOOST_CHECK( value == 0x23 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 2 );

  BOOST_CHECK_THROW(
    boost::ignore_unused( RawData_getInt< uint8_t, boost::endian::order::big >( ArincSupport::ConstRawDataSpan{} ) ),
    std::out_of_range );
}

//! getInt conversion test for uint8_t little endian
BOOST_AUTO_TEST_CASE( getInt8LittleEndianTest )
{
  ConstRawDataSpan data{ rawDataSpan };

  uint8_t value{};
  std::tie( data, value ) = RawData_getInt< uint8_t, boost::endian::order::little > ( data );
  BOOST_CHECK( value == 0x01 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 1 );

  std::tie( data, value ) = RawData_getInt< uint8_t, boost::endian::order::little > ( data );
  BOOST_CHECK( value == 0x23 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 2 );

  BOOST_CHECK_THROW(
    boost::ignore_unused( RawData_getInt< uint8_t, boost::endian::order::little >( ArincSupport::ConstRawDataSpan{} ) ),
    std::out_of_range );
}

//! getInt conversion test for uint16_t big endian
BOOST_AUTO_TEST_CASE( getInt16BigEndianTest )
{
  ConstRawDataSpan data{ rawDataSpan };

  uint16_t value{};
  std::tie( data, value ) = RawData_getInt< uint16_t, boost::endian::order::big > ( data );

  BOOST_CHECK( value == 0x0123 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 2 );

  std::tie( data, value ) = RawData_getInt< uint16_t, boost::endian::order::big > ( data );
  BOOST_CHECK( value == 0x4567 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 4 );

  BOOST_CHECK_THROW(
    boost::ignore_unused( RawData_getInt< uint16_t, boost::endian::order::big >( ArincSupport::ConstRawDataSpan{} ) ),
    std::out_of_range );
}

//! getInt conversion test for uint16_t
BOOST_AUTO_TEST_CASE( getInt16LittleEndianTest )
{
  ConstRawDataSpan data{ rawDataSpan };

  uint16_t value{};
  std::tie( data, value ) = RawData_getInt< uint16_t, boost::endian::order::little > ( data );

  BOOST_CHECK( value == 0x2301 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 2 );

  std::tie( data, value ) = RawData_getInt< uint16_t, boost::endian::order::little > ( data );
  BOOST_CHECK( value == 0x6745 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 4 );

  BOOST_CHECK_THROW(
    boost::ignore_unused( RawData_getInt< uint16_t, boost::endian::order::little >( ArincSupport::ConstRawDataSpan{} ) ),
    std::out_of_range );
}

//! getInt conversion test for uint32_t big endian
BOOST_AUTO_TEST_CASE( getInt32BigEndianTest )
{
  ConstRawDataSpan data{ rawDataSpan };

  uint32_t value{};
  std::tie( data, value ) = RawData_getInt< uint32_t, boost::endian::order::big > ( data );

  BOOST_CHECK( value == 0x01234567 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 4 );

  std::tie( data, value ) = RawData_getInt< uint32_t, boost::endian::order::big > ( data );
  BOOST_CHECK( value == 0x89ABCDEF );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 8 );

  BOOST_CHECK_THROW(
    boost::ignore_unused( RawData_getInt< uint32_t, boost::endian::order::big >( ArincSupport::ConstRawDataSpan{} ) ),
    std::out_of_range );
}

//! getInt conversion test for uint32_t little endian
BOOST_AUTO_TEST_CASE( getInt32BigLittleTest )
{
  ConstRawDataSpan data{ rawDataSpan };

  uint32_t value{};
  std::tie( data, value ) = RawData_getInt< uint32_t, boost::endian::order::little > ( data );

  BOOST_CHECK( value == 0x67452301 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 4 );

  std::tie( data, value ) = RawData_getInt< uint32_t, boost::endian::order::little > ( data );
  BOOST_CHECK( value == 0xEFCDAB89 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 8 );

  BOOST_CHECK_THROW(
    boost::ignore_unused( RawData_getInt< uint32_t, boost::endian::order::little >( ArincSupport::ConstRawDataSpan{} ) ),
    std::out_of_range );
}

//! getInt conversion test for uint64_t big endian
BOOST_AUTO_TEST_CASE( getInt64BigEndianTest )
{
  ConstRawDataSpan data{ rawDataSpan };

  uint64_t value{};
  std::tie( data, value ) = RawData_getInt< uint64_t, boost::endian::order::big > ( data );

  BOOST_CHECK( value == 0x0123'4567'89AB'CDEF );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 8 );

  std::tie( data, value ) = RawData_getInt< uint64_t, boost::endian::order::big > ( data );
  BOOST_CHECK( value == 0x0F1E'2D3C'4B5A'6978 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 16 );

  BOOST_CHECK_THROW(
    boost::ignore_unused( RawData_getInt< uint64_t, boost::endian::order::big >( ArincSupport::ConstRawDataSpan{} ) ),
    std::out_of_range );
}

//! getInt conversion test for uint64_t little endian
BOOST_AUTO_TEST_CASE( getInt64BigLittleTest )
{
  ConstRawDataSpan data{ rawDataSpan };

  uint64_t value{};
  std::tie( data, value ) = RawData_getInt< uint64_t, boost::endian::order::little > ( data );

  BOOST_CHECK( value == 0xEFCD'AB89'6745'2301 );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 8 );

  std::tie( data, value ) = RawData_getInt< uint64_t, boost::endian::order::little > ( data );
  BOOST_CHECK( value == 0x7869'5A4B'3C2D'1E0F );
  BOOST_CHECK( data.size() == rawDataSpan.size() - 16 );

  BOOST_CHECK_THROW(
    boost::ignore_unused( RawData_getInt< uint64_t, boost::endian::order::little >( ArincSupport::ConstRawDataSpan{} ) ),
    std::out_of_range );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
