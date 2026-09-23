// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Template Class ArincSupport::Description.
 **/

#include <arinc_support/Support.hpp>

#include <arinc_support/Description.hpp>

#include <boost/test/unit_test.hpp>

namespace ArincSupport {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( DescriptionTest )

//! Test Enumeration for Class TestDescription
enum class TestEnumeration
{
  Enum1,
  Enum2,
  Enum3
};

//! Description of TestEnumeration
class TestDescription final : public Description<
  TestDescription,
  TestEnumeration >
{
  public:
    //! Constructor
    TestDescription();
};

TestDescription::TestDescription() :
  Description{
    { "Enum1", TestEnumeration::Enum1 },
    { "Enum2", TestEnumeration::Enum2 },
    { "Enum3", TestEnumeration::Enum3 } }
{
}

//! Unit Test for description by enumeration
BOOST_AUTO_TEST_CASE( descriptionByEnum )
{
  const auto &descr1{ TestDescription::instance().description( TestEnumeration::Enum1 ) };
  BOOST_CHECK( descr1.enumeration == TestEnumeration::Enum1 );
  BOOST_CHECK( descr1.name == "Enum1" );
  BOOST_CHECK( descr1.value == ArincSupport::toUnderlying( TestEnumeration::Enum1 ) );

  const auto &descr2{ TestDescription::instance().description( TestEnumeration::Enum3 ) };
  BOOST_CHECK( descr2.enumeration == TestEnumeration::Enum3 );
  BOOST_CHECK( descr2.name == "Enum3" );
  BOOST_CHECK( descr2.value == ArincSupport::toUnderlying( TestEnumeration::Enum3 ) );

  BOOST_CHECK_THROW(
    // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
    boost::ignore_unused( TestDescription::instance().description( static_cast< TestEnumeration >( 25 ) ) ),
    DescriptionNotFoundException );
}

//! Unit Test for description by value
BOOST_AUTO_TEST_CASE( desciptionByValue )
{
  const auto &descr1{ TestDescription::instance().description( ArincSupport::toUnderlying( TestEnumeration::Enum1 ) ) };
  BOOST_CHECK( descr1.enumeration == TestEnumeration::Enum1 );
  BOOST_CHECK( descr1.name == "Enum1" );
  BOOST_CHECK( descr1.value == ArincSupport::toUnderlying( TestEnumeration::Enum1 ) );

  const auto &descr2{ TestDescription::instance().description( ArincSupport::toUnderlying( TestEnumeration::Enum3 ) ) };
  BOOST_CHECK( descr2.enumeration == TestEnumeration::Enum3 );
  BOOST_CHECK( descr2.name == "Enum3" );
  BOOST_CHECK( descr2.value == ArincSupport::toUnderlying( TestEnumeration::Enum3 ) );

  BOOST_CHECK_THROW(
    boost::ignore_unused( TestDescription::instance().description( 25 ) ),
    DescriptionNotFoundException );
}

//! Unit Test for description by name
BOOST_AUTO_TEST_CASE( desciptionByName )
{
  const auto &descr1{ TestDescription::instance().description( "Enum1" ) };
  BOOST_CHECK( descr1.enumeration == TestEnumeration::Enum1 );
  BOOST_CHECK( descr1.name == "Enum1" );
  BOOST_CHECK( descr1.value == ArincSupport::toUnderlying( TestEnumeration::Enum1 ) );

  const auto &descr2{  TestDescription::instance().description( "Enum3" ) };
  BOOST_CHECK( descr2.enumeration == TestEnumeration::Enum3 );
  BOOST_CHECK( descr2.name == "Enum3" );
  BOOST_CHECK( descr2.value == ArincSupport::toUnderlying( TestEnumeration::Enum3 ) );

  BOOST_CHECK_THROW(
    boost::ignore_unused( TestDescription::instance().description( "XXX" ) ),
    DescriptionNotFoundException );
}

//! Unit Test for value by enumeration
BOOST_AUTO_TEST_CASE( valueByEnum )
{
  const auto value1{ TestDescription::instance().value( TestEnumeration::Enum1 ) };
  BOOST_CHECK(
    value1.has_value()
    && ( *value1 == ArincSupport::toUnderlying( TestEnumeration::Enum1 ) ) );

  const auto value2{ TestDescription::instance().value( TestEnumeration::Enum3 ) };
  BOOST_CHECK(
    value2.has_value()
    && ( *value2 == ArincSupport::toUnderlying( TestEnumeration::Enum3 ) ) );

  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  const auto value3{ TestDescription::instance().value( static_cast< TestEnumeration >( 25 ) ) };
  BOOST_CHECK( !value3.has_value() );
}

//! Unit Test for value by name
BOOST_AUTO_TEST_CASE( valueByName )
{
  const auto value1{ TestDescription::instance().value( "Enum1" ) };
  BOOST_CHECK(
    value1.has_value()
    && ( *value1 == ArincSupport::toUnderlying( TestEnumeration::Enum1 ) ) );

  const auto value2{ TestDescription::instance().value( "Enum3" ) };
  BOOST_CHECK(
    value2.has_value()
    && ( *value2 == ArincSupport::toUnderlying( TestEnumeration::Enum3 ) ) );

  const auto value3{ TestDescription::instance().value( "XXX" ) };
  BOOST_CHECK( !value3.has_value() );
}

//! Unit Test for enumeration by value
BOOST_AUTO_TEST_CASE( enumerationByValue )
{
  const auto enum1{ TestDescription::instance().enumeration( ArincSupport::toUnderlying( TestEnumeration::Enum1 ) ) };
  BOOST_CHECK(
    enum1.has_value()
    && ( *enum1 == TestEnumeration::Enum1 ) );

  const auto enum2{ TestDescription::instance().enumeration( ArincSupport::toUnderlying( TestEnumeration::Enum3 ) ) };
  BOOST_CHECK(
    enum2.has_value()
    && ( *enum2 == TestEnumeration::Enum3 ) );

  const auto enum3{ TestDescription::instance().enumeration( 25 ) };
  BOOST_CHECK( !enum3.has_value() );
}

//! Unit Test for enumeration by name
BOOST_AUTO_TEST_CASE( enumerationByName )
{
  const auto enum1{ TestDescription::instance().enumeration( "Enum1" ) };
  BOOST_CHECK( enum1.has_value() && ( *enum1 == TestEnumeration::Enum1 ) );

  const auto enum2{ TestDescription::instance().enumeration( "Enum3" ) };
  BOOST_CHECK( enum2.has_value() && ( *enum2 == TestEnumeration::Enum3 ) );

  const auto enum3{ TestDescription::instance().enumeration( "XXX" ) };
  BOOST_CHECK( !enum3.has_value() );
}

//! Unit Test for name by value
BOOST_AUTO_TEST_CASE( nameByValue )
{
  const auto name1{ TestDescription::instance().name( ArincSupport::toUnderlying( TestEnumeration::Enum1 ) ) };
  BOOST_CHECK( !name1.empty() );
  BOOST_CHECK( name1 == "Enum1" );

  const auto name2{ TestDescription::instance().name( ArincSupport::toUnderlying( TestEnumeration::Enum3 ) ) };
  BOOST_CHECK( !name2.empty() );
  BOOST_CHECK( name2 == "Enum3" );

  const auto name3{ TestDescription::instance().name( 25 ) };
  BOOST_CHECK( name3.empty() );
}

//! Unit Test for name by enumeration
BOOST_AUTO_TEST_CASE( nameByEnum )
{
  const auto name1{ TestDescription::instance().name( TestEnumeration::Enum1 ) };
  BOOST_CHECK( !name1.empty() );
  BOOST_CHECK( name1 == "Enum1" );

  const auto name2{ TestDescription::instance().name( TestEnumeration::Enum3 ) };
  BOOST_CHECK( !name2.empty() );
  BOOST_CHECK( name2 == "Enum3" );

  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  const auto name3{ TestDescription::instance().name( static_cast< TestEnumeration >( 25 ) ) };
  BOOST_CHECK( name3.empty() );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
