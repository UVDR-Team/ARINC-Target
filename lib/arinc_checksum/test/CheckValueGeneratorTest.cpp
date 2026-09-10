// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for class ArincChecksum::CheckValueGenerator.
 **/

#include <arinc_checksum/CheckValueGenerator.hpp>
#include <arinc_checksum/CheckValue.hpp>
#include <arinc_checksum/Arinc645Exception.hpp>

#include <arinc_checksum/test/Arinc645TestData.hpp>

#include <arinc_support/RawData.hpp>

#include <boost/test/unit_test.hpp>

namespace ArincChecksum {

BOOST_AUTO_TEST_SUITE( Arinc645Test )
BOOST_AUTO_TEST_SUITE( CheckValueGeneratorTest )

//! Create Generator Test
BOOST_AUTO_TEST_CASE( create )
{
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::NotUsed ) );
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::Crc8 ) );
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::Crc16 ) );
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::Crc32 ) );
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::Md5 ) );
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::Sha1 ) );
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::Sha256 ) );
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::Sha512 ) );
  BOOST_CHECK( CheckValueGenerator::create( CheckValueType::Crc64 ) );
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  BOOST_CHECK( !CheckValueGenerator::create( CheckValueType{ 25 } ) );
}

//! Not Used test
BOOST_AUTO_TEST_CASE( notUsed )
{
  const auto notUsed{ CheckValueGenerator::checkValue( CheckValueType::NotUsed, ArincSupport::ConstRawDataSpan{} ) };
  BOOST_CHECK( notUsed.has_value() && ( notUsed->type() == CheckValueType::NotUsed ) && notUsed->value().empty() );
}

/**
 * @brief Check CRC Test
 *
 * @tparam CrcT
 *   CRC Type
 * @tparam CheckValueType
 *   Check Value Type
 *
 * @param[in] data
 *   Test Data
 * @param[in] expectedCrc
 *   Expected CRC Data
 **/
template< std::unsigned_integral CrcT, CheckValueType CheckValueType >
static void CheckCrc( ArincSupport::ConstRawDataSpan data, CrcT expectedCrc )
{
  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType, data ) };

  BOOST_CHECK(
    checkValue.has_value()
    && ( checkValue->type() == CheckValueType )
    && ( sizeof( CrcT ) == checkValue->value().size() ) );
  const auto &&realCheckValue{ checkValue.value_or( CheckValue::NoCheckValue ) };
  const auto rawCheckValue{ realCheckValue.value() };
  BOOST_CHECK( rawCheckValue.size() == sizeof( CrcT ) );
  auto [ _, crc ]{ ArincSupport::RawData_getInt< CrcT >( rawCheckValue ) };
  BOOST_CHECK_EQUAL( expectedCrc, crc );
}

//! MD5 test
BOOST_AUTO_TEST_CASE( md5 )
{
  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Md5, getTestData( TestData::t12a ) ) };

  BOOST_CHECK(
    checkValue.has_value()
    && ( checkValue->type() == CheckValueType::Md5 )
    && ( 16 == checkValue->value().size() ) );
}

//! SHA1 test
BOOST_AUTO_TEST_CASE( sha1 )
{
  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Sha1, getTestData( TestData::t12a ) ) };

  BOOST_CHECK(
    checkValue.has_value()
    && ( checkValue->type() == CheckValueType::Sha1 )
    && ( 20 == checkValue->value().size() ) );
}

//! SHA256 test
BOOST_AUTO_TEST_CASE( sha256 )
{
  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Sha256, getTestData( TestData::t12a ) ) };

  BOOST_CHECK(
    checkValue.has_value()
    && ( checkValue->type() == CheckValueType::Sha256 )
    && ( 32 == checkValue->value().size() ) );
}

//! SHA512 test
BOOST_AUTO_TEST_CASE( sha512 )
{
  const auto checkValue{ CheckValueGenerator::checkValue( CheckValueType::Sha512, getTestData( TestData::t12a ) ) };

  BOOST_CHECK(
    checkValue.has_value()
    && ( checkValue->type() == CheckValueType::Sha512 )
    && ( 64 == checkValue->value().size() ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
