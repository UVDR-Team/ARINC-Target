// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc649::CheckValueGenerator CRC-64.
 **/

#include <arinc_649/CheckValueGenerator.hpp>
#include <arinc_649/CheckValue.hpp>
#include <arinc_649/Arinc649Exception.hpp>

#include <arinc_649/test/Arinc649TestData.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc649 {

BOOST_AUTO_TEST_SUITE( Arinc649Test )
BOOST_AUTO_TEST_SUITE( CheckValueGeneratorCrc64Test )

//! CRC-64 test
BOOST_AUTO_TEST_CASE( crc64_t01a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc64, getTestData( TestData::t01a ) ) };
  BOOST_CHECK( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc64 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint64_t ) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint64_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK_EQUAL( crc, getTestDataCrc64( TestData::t01a ) );
}

//! CRC-64 test
BOOST_AUTO_TEST_CASE( crc64_t02a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc64, getTestData( TestData::t02a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc64 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint64_t ) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint64_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK_EQUAL( crc, getTestDataCrc64( TestData::t02a ) );
}

//! CRC-64 test
BOOST_AUTO_TEST_CASE( crc64_t03a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc64, getTestData( TestData::t03a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc64 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint64_t ) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint64_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK_EQUAL( crc, getTestDataCrc64( TestData::t03a ) );
}

//! CRC-64 test
BOOST_AUTO_TEST_CASE( crc64_t04a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc64, getTestData( TestData::t04a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc64 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint64_t ) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint64_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK_EQUAL( crc, getTestDataCrc64( TestData::t04a ) );
}

//! CRC-64 test
BOOST_AUTO_TEST_CASE( crc64_t11a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc64, getTestData( TestData::t11a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc64 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint64_t ) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint64_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK_EQUAL( crc, getTestDataCrc64( TestData::t11a ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
