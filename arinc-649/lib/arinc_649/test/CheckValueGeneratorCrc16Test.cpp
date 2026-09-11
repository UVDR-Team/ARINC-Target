// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc649::CheckValueGenerator CRC-16.
 **/

#include <arinc_649/CheckValueGenerator.hpp>
#include <arinc_649/CheckValue.hpp>
#include <arinc_649/Arinc649Exception.hpp>

#include <arinc_649/test/Arinc649TestData.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc649 {

BOOST_AUTO_TEST_SUITE( Arinc649Test )
BOOST_AUTO_TEST_SUITE( CheckValueGeneratorCrc16Test )

//! CRC16 test
BOOST_AUTO_TEST_CASE( crc16_t01a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc16, getTestData( TestData::t01a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc16 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint16_t ) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint16_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK_EQUAL( crc, getTestDataCrc16( TestData::t01a ) );
}

//! CRC16 test
BOOST_AUTO_TEST_CASE( crc16_t02a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc16, getTestData( TestData::t02a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc16 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint16_t ) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint16_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK_EQUAL( crc, getTestDataCrc16( TestData::t02a ) );
}

//! CRC16 test
BOOST_AUTO_TEST_CASE( crc16_t03a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc16, getTestData( TestData::t03a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc16 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint16_t ) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint16_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK_EQUAL( crc, getTestDataCrc16( TestData::t03a ) );
}

//! CRC16 test
BOOST_AUTO_TEST_CASE( crc16_t04a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc16, getTestData( TestData::t04a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc16 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint16_t) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint16_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK( checkValue.has_value() && crc == getTestDataCrc16( TestData::t04a ) );
}

//! CRC16 test
BOOST_AUTO_TEST_CASE( crc16_t11a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc16, getTestData( TestData::t11a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc16 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == sizeof( uint16_t) );
  auto [ _, crc ]{ Helper::RawData_getInt< uint16_t >( checkValue.value_or( CheckValue::NoCheckValue ).value() ) };
  BOOST_CHECK( checkValue.has_value() && crc == getTestDataCrc16( TestData::t11a ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
