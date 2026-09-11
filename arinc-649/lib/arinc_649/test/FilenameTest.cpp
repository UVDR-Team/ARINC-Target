// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc649 Filename.
 **/

#include <arinc_649/Filename.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc649 {

BOOST_AUTO_TEST_SUITE( Arinc649Test )
BOOST_AUTO_TEST_SUITE( FilenameTest )

//! Check Value to CRC 16
BOOST_AUTO_TEST_CASE( FilenameTest )
{
  BOOST_CHECK( !Filename_check( {} ) );
  BOOST_CHECK( !Filename_check( "." ) );
  BOOST_CHECK( !Filename_check( ".." ) );
  BOOST_CHECK( Filename_check( "..." ) );
  BOOST_CHECK( Filename_check( ".A" ) );
  BOOST_CHECK( Filename_check( "..A" ) );
  BOOST_CHECK( !Filename_check( "ABC\"ABC" ) );
  BOOST_CHECK( !Filename_check( "ABC~ABC" ) );
  BOOST_CHECK( Filename_check( "Hello_World.txt" ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
