// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for class Helper::Bools.
 **/

#include <helper/Bool.hpp>

#include <boost/test/unit_test.hpp>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( BoolTest )

//! Find value test for Boolean Enumeration
BOOST_AUTO_TEST_CASE( findValueBooleanEnum )
{
  BOOST_CHECK( Bools::instance().value( Bool::False ) == 0 );
  BOOST_CHECK( Bools::instance().value( Bool::True ) == 1 );
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  BOOST_CHECK( !Bools::instance().value( static_cast< Bool>( 4 ) ) );
}

//! Find value test for String
BOOST_AUTO_TEST_CASE( findValueString )
{
  using namespace std::literals;

  BOOST_CHECK( Bools::instance().value( "false"s ) == 0 );
  BOOST_CHECK( Bools::instance().value( "true"s ) == 1 );
  BOOST_CHECK( !Bools::instance().value( "XXX"s ) );
}

//! Find value test for bool
BOOST_AUTO_TEST_CASE( findValueBoolean )
{
  BOOST_CHECK( Bools::instance().value( false ) == 0 );
  BOOST_CHECK( Bools::instance().value( true ) == 1 );
}

//! Find enum test
BOOST_AUTO_TEST_CASE( findEnum1 )
{
  BOOST_CHECK( Bools::instance().enumeration( static_cast< uint32_t>( 0UL ) ) == Bool::False );
  BOOST_CHECK( Bools::instance().enumeration( static_cast< uint32_t>( 1UL ) ) == Bool::True );
  BOOST_CHECK( !Bools::instance().enumeration( static_cast< uint32_t>( 2UL ) ) );
  BOOST_CHECK( !Bools::instance().enumeration( static_cast< uint32_t>( 0xFFFF'FFFFUL ) ) );
}

//! Find enum test
BOOST_AUTO_TEST_CASE( findEnum2 )
{
  BOOST_CHECK( Bools::instance().enumeration( "false" ) == Bool::False );
  BOOST_CHECK( Bools::instance().enumeration( "true" ) == Bool::True );
  BOOST_CHECK( !Bools::instance().enumeration( "invalid" ) );
  BOOST_CHECK( !Bools::instance().enumeration( "XXX" ) );
}

//! Find bool test
BOOST_AUTO_TEST_CASE( findBool1 )
{
  BOOST_CHECK( Bools::instance().boolean( static_cast< uint32_t>( 0UL ) ) == false );
  BOOST_CHECK( Bools::instance().boolean( static_cast< uint32_t>( 1UL ) ) == true );
  BOOST_CHECK_THROW(
    boost::ignore_unused( Bools::instance().boolean( static_cast< uint32_t>( 2UL ) ) ),
    DescriptionNotFoundException );
  BOOST_CHECK_THROW(
    boost::ignore_unused( Bools::instance().boolean( static_cast< uint32_t>( 0xFFFF'FFFFUL ) ) ),
    DescriptionNotFoundException );
}

//! Find bool test
BOOST_AUTO_TEST_CASE( findBool2 )
{
  BOOST_CHECK( Bools::instance().boolean( "false" ) == false );
  BOOST_CHECK( Bools::instance().boolean( "true" ) == true );
  BOOST_CHECK_THROW(
    boost::ignore_unused( Bools::instance().boolean( "invalid" ) ),
    DescriptionNotFoundException );
  BOOST_CHECK_THROW(
    boost::ignore_unused( Bools::instance().boolean( "XXX" ) ),
    DescriptionNotFoundException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
