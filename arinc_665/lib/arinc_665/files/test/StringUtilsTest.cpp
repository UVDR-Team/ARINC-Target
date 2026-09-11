// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Module Arinc665::Files::StringUtils.
 **/

#include <arinc_665/files/StringUtils.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc665::Files {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( SringUtilsTest )

//! Decode String Test
BOOST_AUTO_TEST_CASE( decodeString )
{
  Helper::ConstRawDataSpan remaining;
  std::string out;

  const uint8_t rawStr1[]{ 0x00, 0x00 };
  std::tie( remaining, out ) = StringUtils_decodeString( std::as_bytes( std::span{ rawStr1 } ) );
  BOOST_CHECK( remaining.empty() );
  BOOST_CHECK( out.empty() );

  const uint8_t rawStr2[]{  0x00, 0x04, 'T', 'e', 's', 't' };
  std::tie( remaining, out ) = StringUtils_decodeString( std::as_bytes( std::span{ rawStr2 } ) );
  BOOST_CHECK( remaining.empty() );
  BOOST_CHECK( out == "Test" );

  const uint8_t rawStr3[]{ 0x00, 0x05, 'T', 'e', 's', 't', '1', 0x00 };
  std::tie( remaining, out ) = StringUtils_decodeString( std::as_bytes( std::span{ rawStr3 } ) );
  BOOST_CHECK( remaining.empty() );
  BOOST_CHECK( out == "Test1" );

  const uint8_t rawStr4[]{ 0x00, 0x05, 'T', 'e', 's', 't', '1', 1U };
  BOOST_CHECK_THROW( StringUtils_decodeString( std::as_bytes( std::span{ rawStr4 } ) ), Arinc665Exception );
}

//! Encode String Test
BOOST_AUTO_TEST_CASE( encodeString )
{
  const uint8_t expected1[]{ 0x00, 0x04, 'T', 'e', 's', 't' };
  BOOST_CHECK( std::ranges::equal( StringUtils_encodeString( "Test" ), std::as_bytes( std::span{ expected1 } ) ) );

  const uint8_t expected2[]{ 0x00, 0x05, 'T', 'e', 's', 't', '1', 0x00 };
  BOOST_CHECK( std::ranges::equal( StringUtils_encodeString( "Test1" ), std::as_bytes( std::span{ expected2 } ) ) );

  const uint8_t expected3[]{ 0x00, 0x00 };
  BOOST_CHECK( std::ranges::equal( StringUtils_encodeString( "" ), std::as_bytes( std::span{ expected3 } ) ) );
}

//! Decode Strings Test
BOOST_AUTO_TEST_CASE( decodeStrings )
{
  Helper::ConstRawDataSpan remaining;
  std::list< std::string > out;

  const uint8_t rawStringList1[]{
    0x00, 0x03,
    0x00, 0x04, 'T', 'e', 's', 't',
    0x00, 0x05, 'T', 'e', 's', 't', '1', 0x00,
    0x00, 0x00 };
  std::tie( remaining, out ) = StringUtils_decodeStrings( std::as_bytes( std::span{ rawStringList1 } ) );

  BOOST_CHECK( remaining.empty() );
  BOOST_CHECK( out.size() == 3 );
  BOOST_CHECK( *out.begin() == "Test" );
  BOOST_CHECK( *std::next( out.begin() ) == "Test1" );
  BOOST_CHECK( std::next( out.begin(), 2 )->empty() );

  const uint8_t rawStringList2[]{ 0x00, 0x00 };
  std::tie( remaining, out ) = StringUtils_decodeStrings( std::as_bytes( std::span{ rawStringList2 } ) );
  BOOST_CHECK( remaining.empty() );
  BOOST_CHECK( out.empty() );
}

//! Encode Strings Test
BOOST_AUTO_TEST_CASE( encodeStrings )
{
  const uint8_t expected1[]{ 0x00, 0x00 };
  BOOST_CHECK( std::ranges::equal( StringUtils_encodeStrings( {} ), std::as_bytes( std::span{ expected1 } ) ) );

  const uint8_t expected2[]{
    0x00, 0x03,
    0x00, 0x04, 'T', 'e', 's', 't',
    0x00, 0x05, 'T', 'e', 's', 't', '1', 0x00,
    0x00, 0x00 };
  BOOST_CHECK(
    std::ranges::equal(
      StringUtils_encodeStrings( { "Test", "Test1", "" } ),
      std::as_bytes( std::span{ expected2 } ) ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
