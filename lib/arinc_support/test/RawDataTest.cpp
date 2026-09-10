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
#include <vector>

namespace ArincSupport {

BOOST_AUTO_TEST_SUITE( HelperTest)
BOOST_AUTO_TEST_SUITE( RawDataTest )

//! Test String
static constexpr std::string_view string{ "TEST" };
//! Test String as raw Data
static const auto rawString{ RawData_asRawData( string ) };

//! Raw to String conversion
BOOST_AUTO_TEST_CASE( getStringTest )
{
  auto [ remaining1, string1 ]{ RawData_getString( rawString, 0 ) };
  BOOST_CHECK( remaining1.size() == rawString.size() );
  BOOST_CHECK( string1.empty() );

  auto [ remaining2, string2 ]{ RawData_getString( rawString, 2 ) };
  BOOST_CHECK( remaining2.size() == rawString.size() - 2 );
  BOOST_CHECK( string2 == "TE" );

  auto [ remaining3, string3 ]{ RawData_getString( rawString, 4 ) };
  BOOST_CHECK( remaining3.empty() );
  BOOST_CHECK( string3 == "TEST" );
}

//! Raw to String conversion
BOOST_AUTO_TEST_CASE( setStringTest )
{
  std::array< std::byte, 4 > rawStringData{};

  const auto remaining{ RawData_setString( rawStringData, string ) };
  BOOST_CHECK( remaining.empty() );
  BOOST_CHECK( std::ranges::equal( rawStringData, rawString ) );
}

//! Raw Data as String representation
BOOST_AUTO_TEST_CASE( asStringTest )
{
  BOOST_CHECK( RawData_asString( ConstRawDataSpan{} ).empty() );
  BOOST_CHECK( RawData_asString( ArincSupport::RawData_asRaw( "TEST" ) ) == "TEST" );
}

//! Raw Data as String representation
BOOST_AUTO_TEST_CASE( asStringTestWithLength )
{
  BOOST_CHECK( RawData_asString( ArincSupport::RawData_asRaw( "TEST" ), 0 ).empty() );
  BOOST_CHECK( RawData_asString( ArincSupport::RawData_asRaw( "TEST" ), 2 ) == "TE" );
  BOOST_CHECK( RawData_asString( ArincSupport::RawData_asRaw( "TEST" ), 4 ) == "TEST" );
}

//! Raw to String conversion
BOOST_AUTO_TEST_CASE( asRawStringTest )
{
  auto rawStringData{ RawData_asRaw( string ) };
  BOOST_CHECK( std::ranges::equal( rawStringData, rawString ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
