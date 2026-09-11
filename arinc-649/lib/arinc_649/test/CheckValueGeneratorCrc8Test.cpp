// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class Arinc649::CheckValueGenerator CRC-8.
 **/

#include <arinc_649/CheckValueGenerator.hpp>
#include <arinc_649/CheckValue.hpp>
#include <arinc_649/Arinc649Exception.hpp>

#include <arinc_649/test/Arinc649TestData.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc649 {

BOOST_AUTO_TEST_SUITE( Arinc649Test )
BOOST_AUTO_TEST_SUITE( CheckValueGeneratorCrc8Test )

//! CRC-8 test
BOOST_AUTO_TEST_CASE( crc8_t01a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc8, getTestData( TestData::t01a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc8 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == 2 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 0 ] == 0x0_b );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 1 ] == std::byte{ getTestDataCrc8( TestData::t01a ) } );
}

//! CRC-8 test
BOOST_AUTO_TEST_CASE( crc8_t02a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc8, getTestData( TestData::t02a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc8 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == 2 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 0 ] == 0x0_b );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 1 ] == std::byte{ getTestDataCrc8( TestData::t02a ) } );
}

//! CRC-8 test
BOOST_AUTO_TEST_CASE( crc8_t03a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc8, getTestData( TestData::t03a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc8 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == 2 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 0 ] == 0x0_b );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 1 ] == std::byte{ getTestDataCrc8( TestData::t03a ) } );
}

//! CRC-8 test
BOOST_AUTO_TEST_CASE( crc8_t12a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc8, getTestData( TestData::t12a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc8 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == 2 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 0 ] == 0x0_b );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 1 ] == std::byte{ getTestDataCrc8( TestData::t12a ) } );
}

//! CRC-8 test
BOOST_AUTO_TEST_CASE( crc8_t13a )
{
  using Helper::operator""_b;

  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Crc8, getTestData( TestData::t13a ) ) };
  BOOST_REQUIRE( checkValue.has_value() );
  BOOST_CHECK( checkValue.has_value() && checkValue->type() == CheckValueType::Crc8 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value().size() == 2 );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 0 ] == 0x0_b );
  BOOST_CHECK( checkValue.has_value() && checkValue->value() [ 1 ] == std::byte{ getTestDataCrc8( TestData::t13a ) } );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
