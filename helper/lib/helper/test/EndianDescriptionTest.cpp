// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for class Helper::EndianDescription.
 **/

#include <helper/EndianDescription.hpp>

#include <boost/test/unit_test.hpp>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( EndianTest )

//! Find value test for Enumeration
BOOST_AUTO_TEST_CASE( findValueEnum )
{
  BOOST_CHECK( EndianDescription::instance().value( std::endian::big ) == std::to_underlying( std::endian::big ) );
  BOOST_CHECK( EndianDescription::instance().value( std::endian::little ) == std::to_underlying( std::endian::little ) );
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  BOOST_CHECK( !EndianDescription::instance().value( static_cast< std::endian >( 4 ) ) );
}

//! Find Value by Name Test
BOOST_AUTO_TEST_CASE( findValuenName )
{
  BOOST_CHECK( EndianDescription::instance().value( "big" ) == std::to_underlying( std::endian::big ) );
  BOOST_CHECK( EndianDescription::instance().value( "little" ) == std::to_underlying( std::endian::little ) );
}

//! Find Enumeration by Value Test
BOOST_AUTO_TEST_CASE( findEnumerationValue )
{
  BOOST_CHECK( EndianDescription::instance().enumeration( std::to_underlying( std::endian::big ) ) == std::endian::big );
  BOOST_CHECK( EndianDescription::instance().enumeration( std::to_underlying( std::endian::little ) ) == std::endian::little );
}

//! Find Enumeration by Name Test
BOOST_AUTO_TEST_CASE( findEnumerationName )
{
  BOOST_CHECK( EndianDescription::instance().enumeration( "big" ) == std::endian::big );
  BOOST_CHECK( EndianDescription::instance().enumeration( "little" ) == std::endian::little );
}

//! Convert std::endian to to boost::endian::order
BOOST_AUTO_TEST_CASE( stdToBoost )
{
  BOOST_CHECK( EndianDescription::toEndian( std::endian::big ) == boost::endian::order::big );
  BOOST_CHECK( EndianDescription::toEndian( std::endian::little ) == boost::endian::order::little );
  BOOST_CHECK( EndianDescription::toEndian( std::endian::native ) == boost::endian::order::native );
}

//! Convert boost::endian::order to std::endian
BOOST_AUTO_TEST_CASE( boostToStd )
{
  BOOST_CHECK( EndianDescription::toEndian( boost::endian::order::big ) == std::endian::big );
  BOOST_CHECK( EndianDescription::toEndian( boost::endian::order::little ) == std::endian::little );
  BOOST_CHECK( EndianDescription::toEndian( boost::endian::order::native ) == std::endian::native );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
