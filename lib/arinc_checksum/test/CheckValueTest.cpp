// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Module Arinc645 Utils.
 **/

#include <arinc_checksum/CheckValue.hpp>
#include <arinc_checksum/Arinc645Exception.hpp>

#include <arinc_support/RawData.hpp>

#include <boost/test/unit_test.hpp>

namespace ArincChecksum {

BOOST_AUTO_TEST_SUITE( Arinc645Test )
BOOST_AUTO_TEST_SUITE( CheckValueTest )

//! Check Value to CRC-16
BOOST_AUTO_TEST_CASE( checkValueCrc16 )
{
  using ArincSupport::operator""_b;

  BOOST_CHECK( CheckValue::crc16( { CheckValueType::Crc16, { 0x01_b, 0x23_b } } ) == 0x0123 );
  BOOST_CHECK( !CheckValue::crc16( { CheckValueType::Crc16, std::vector< std::byte >{} } ) );
  BOOST_CHECK( !CheckValue::crc16( { CheckValueType::Crc16, { 0x01_b, 0x23_b, 0x45_b } } ) );
  BOOST_CHECK( !CheckValue::crc16( { CheckValueType::Crc32, { 0x01_b, 0x23_b } } ) );
}

//! CRC-16 to Check Value
BOOST_AUTO_TEST_CASE( crc16CheckValue )
{
  using ArincSupport::operator""_b;

  BOOST_CHECK( ( CheckValue::crc16( 0x0123 ) == CheckValue{ CheckValueType::Crc16, { 0x01_b, 0x23_b } } ) );
}

//! Check Value Construct from String Test
BOOST_AUTO_TEST_CASE( ConstructFromString )
{
  using ArincSupport::operator""_b;

  BOOST_CHECK( (
    CheckValue{ CheckValueType::Crc8, "0023" }
      == CheckValue{ CheckValueType::Crc8, std::vector< std::byte >{ 0x00_b, 0x23_b } } ) );
  BOOST_CHECK( (
    CheckValue{ CheckValueType::Crc16, "0123" }
      == CheckValue{ CheckValueType::Crc16, std::vector< std::byte >{ 0x01_b, 0x23_b } } ) );
  BOOST_CHECK( (
    CheckValue{ CheckValueType::Crc32, "01234567" }
      == CheckValue{ CheckValueType::Crc32, std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b } } ) );
  BOOST_CHECK( (
    CheckValue{ CheckValueType::Crc64, "0123456789ABCDEF" }
      == CheckValue{
        CheckValueType::Crc64,
        std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b } } ) );
  BOOST_CHECK( (
    CheckValue{ CheckValueType::Md5, "0123456789ABCDEF0123456789ABCDEF" }
      == CheckValue{
        CheckValueType::Md5,
      std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b, 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b } } ) );

  BOOST_CHECK( !( CheckValue{ CheckValueType::Crc16, "001234" } ) );
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  BOOST_CHECK( !( CheckValue{ CheckValueType{ 99 }, "0012" } ) );
}

//! CheckValue_toString Test
BOOST_AUTO_TEST_CASE( toString )
{
  using ArincSupport::operator""_b;

  BOOST_CHECK( ( CheckValue{ CheckValueType::Crc8, std::vector< std::byte >{ 0x00_b, 0x23_b } }.toString() == "0023" ) );
  BOOST_CHECK( ( CheckValue{ CheckValueType::Crc16, std::vector< std::byte >{ 0x01_b, 0x23_b } }.toString() == "0123" ) );
  BOOST_CHECK( ( CheckValue{ CheckValueType::Crc32, std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b } }.toString() == "01234567" ) );
  BOOST_CHECK( ( CheckValue{ CheckValueType::Crc64, std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b } }.toString() == "0123456789ABCDEF" ) );
  BOOST_CHECK( ( CheckValue{ CheckValueType::Md5, std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b, 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b } }.toString() == "0123456789ABCDEF0123456789ABCDEF" ) );
  BOOST_CHECK( ( CheckValue{ CheckValueType::Sha1, std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b } }.toString() == "0123456789ABCDEF" ) );
  BOOST_CHECK( ( CheckValue{ CheckValueType::Sha256, std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b } }.toString() == "0123456789ABCDEF" ) );
  BOOST_CHECK( ( CheckValue{ CheckValueType::Sha512, std::vector< std::byte >{ 0x01_b, 0x23_b, 0x45_b, 0x67_b, 0x89_b, 0xAB_b, 0xCD_b, 0xEF_b } }.toString() == "0123456789ABCDEF" ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
