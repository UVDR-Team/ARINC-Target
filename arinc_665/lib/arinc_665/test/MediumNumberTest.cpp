// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for the class Arinc665::MediumNumber.
 **/

#include <arinc_665/Arinc665Exception.hpp>
#include <arinc_665/MediumNumber.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc665 {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( MediumNumberTest )

//! Check Behaviour of default initialised medium numbers
BOOST_AUTO_TEST_CASE( defaultInitialised )
{
  MediumNumber mediumNumber;
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "001" );
  BOOST_CHECK( mediumNumber.toString()  == "001" );
}

//! Check Behaviour of value initialised medium numbers
BOOST_AUTO_TEST_CASE( valueInitialised )
{
  const MediumNumber mediumNumber0{ 0U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber0 ) == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber0 )  == "001" );
  BOOST_CHECK( mediumNumber0.toString()  == "001" );

  const MediumNumber mediumNumber1{ 1U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber1 ) == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber1 )  == "001" );
  BOOST_CHECK( mediumNumber1.toString()  == "001" );

  const MediumNumber mediumNumber255{ 255U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber255 ) == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber255 )  == "255" );
  BOOST_CHECK( mediumNumber255.toString()  == "255" );
}

//! Pre Decrement Test
BOOST_AUTO_TEST_CASE( toString )
{
  BOOST_CHECK( static_cast< std::string >( MediumNumber{ 1U } )  == "001" );
  BOOST_CHECK( static_cast< std::string >( MediumNumber{ 99U } )  == "099" );
  BOOST_CHECK( static_cast< std::string >( MediumNumber{ 255U } )  == "255" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
