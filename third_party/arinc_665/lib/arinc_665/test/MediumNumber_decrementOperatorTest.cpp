// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for the Class Arinc665::MediumNumber.
 **/

#include <arinc_665/MediumNumber.hpp>
#include <arinc_665/Arinc665Exception.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc665 {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( MediumNumberTest )

//! Pre Decrement Test
BOOST_AUTO_TEST_CASE( preDecrement )
{
  MediumNumber mediumNumber{ 255U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "255" );
  BOOST_CHECK( mediumNumber.toString()  == "255" );

  auto mediumNumberSame{ --mediumNumber };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 254U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "254" );
  BOOST_CHECK( mediumNumber.toString()  == "254" );

  BOOST_CHECK( static_cast< uint8_t >( mediumNumberSame )  == 254U );
  BOOST_CHECK( static_cast< std::string >( mediumNumberSame )  == "254" );
  BOOST_CHECK( mediumNumberSame.toString()  == "254" );
}

//! Pre Decrement Test - underrun
BOOST_AUTO_TEST_CASE( preDecrementUnderrun )
{
  MediumNumber mediumNumber{ 1U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "001" );
  BOOST_CHECK( mediumNumber.toString()  == "001" );

  auto mediumNumberSame{ --mediumNumber };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "001" );
  BOOST_CHECK( mediumNumber.toString()  == "001" );

  BOOST_CHECK( static_cast< uint8_t >( mediumNumberSame )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumberSame )  == "001" );
  BOOST_CHECK( mediumNumberSame.toString()  == "001" );
}

//! Post Decrement Test
BOOST_AUTO_TEST_CASE( postDecrement )
{
  MediumNumber mediumNumber{ 255U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "255" );
  BOOST_CHECK( mediumNumber.toString()  == "255" );

  auto mediumNumberOld{ mediumNumber-- };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 254U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "254" );
  BOOST_CHECK( mediumNumber.toString()  == "254" );

  BOOST_CHECK( static_cast< uint8_t >( mediumNumberOld )  == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumberOld )  == "255" );
  BOOST_CHECK( mediumNumberOld.toString()  == "255" );
}

//! Post Decrement Test - underrun
BOOST_AUTO_TEST_CASE( postDecrementUnderrun )
{
  MediumNumber mediumNumber{ 1U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "001" );
  BOOST_CHECK( mediumNumber.toString()  == "001" );

  auto mediumNumberOld{ mediumNumber-- };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "001" );
  BOOST_CHECK( mediumNumber.toString()  == "001" );

  BOOST_CHECK( static_cast< uint8_t >( mediumNumberOld )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumberOld )  == "001" );
  BOOST_CHECK( mediumNumberOld.toString()  == "001" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
