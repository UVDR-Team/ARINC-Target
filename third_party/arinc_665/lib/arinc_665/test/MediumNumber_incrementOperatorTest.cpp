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

//! Pre Increment Test
BOOST_AUTO_TEST_CASE( preIncrement )
{
  MediumNumber mediumNumber{ 1 };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "001" );
  BOOST_CHECK( mediumNumber.toString()  == "001" );

  const auto mediumNumberSame{ ++mediumNumber };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 2U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "002" );
  BOOST_CHECK( mediumNumber.toString()  == "002" );

  BOOST_CHECK( static_cast< uint8_t >( mediumNumberSame )  == 2U );
  BOOST_CHECK( static_cast< std::string >( mediumNumberSame )  == "002" );
  BOOST_CHECK( mediumNumberSame.toString()  == "002" );
}

//! Pre Increment Test - overrun
BOOST_AUTO_TEST_CASE( preIncrementOverrun )
{
  MediumNumber mediumNumber{ 255U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "255" );
  BOOST_CHECK( mediumNumber.toString()  == "255" );

  const auto mediumNumberSame{ ++mediumNumber };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "255" );
  BOOST_CHECK( mediumNumber.toString()  == "255" );

  BOOST_CHECK( static_cast< uint8_t >( mediumNumberSame )  == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumberSame )  == "255" );
  BOOST_CHECK( mediumNumberSame.toString()  == "255" );
}

//! Post-Increment Test
BOOST_AUTO_TEST_CASE( postIncrement )
{
  MediumNumber mediumNumber;
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 1U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "001" );
  BOOST_CHECK( mediumNumber.toString()  == "001" );

  const auto oldMediumNumber{ mediumNumber++ };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 2U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "002" );
  BOOST_CHECK( mediumNumber.toString()  == "002" );

  BOOST_CHECK( static_cast< uint8_t >( oldMediumNumber )  == 1U );
  BOOST_CHECK( static_cast< std::string >( oldMediumNumber )  == "001" );
  BOOST_CHECK( oldMediumNumber.toString()  == "001" );
}

//! Post-Increment Test - Overrun
BOOST_AUTO_TEST_CASE( postIncrementOverrun )
{
  MediumNumber mediumNumber{ 255U };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "255" );
  BOOST_CHECK( mediumNumber.toString()  == "255" );

  const auto oldMediumNumber{ mediumNumber++ };
  BOOST_CHECK( static_cast< uint8_t >( mediumNumber )  == 255U );
  BOOST_CHECK( static_cast< std::string >( mediumNumber )  == "255" );
  BOOST_CHECK( mediumNumber.toString()  == "255" );

  BOOST_CHECK( static_cast< uint8_t >( oldMediumNumber )  == 255U );
  BOOST_CHECK( static_cast< std::string >( oldMediumNumber )  == "255" );
  BOOST_CHECK( oldMediumNumber.toString()  == "255" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
