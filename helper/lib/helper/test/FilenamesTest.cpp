// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Module Helper Filenames.
 **/

#include <helper/Filenames.hpp>

#include <boost/test/unit_test.hpp>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( FilenamesTest )

//! Unit Test for description by enumeration
BOOST_AUTO_TEST_CASE( normalizeFilename )
{
  BOOST_CHECK( normaliseFilename( "" ) == "_" );
  BOOST_CHECK( normaliseFilename( "." ) == "_" );
  BOOST_CHECK( normaliseFilename( ".." ) == "__" );
  BOOST_CHECK( normaliseFilename( "..." ) == "..." );

  BOOST_CHECK( normaliseFilename( "CON" ) == "_CON" );
  BOOST_CHECK( normaliseFilename( "CON.txt" ) == "_CON.txt" );
  BOOST_CHECK( normaliseFilename( "CONABC.txt" ) == "CONABC.txt" );

  BOOST_CHECK( normaliseFilename( "NUL" ) == "_NUL" );
  BOOST_CHECK( normaliseFilename( "NULL" ) == "NULL" );

  BOOST_CHECK( normaliseFilename( "abc/def.txt" ) == "abc_def.txt" );
  BOOST_CHECK( normaliseFilename( "abc?def.txt" ) == "abc_def.txt" );

  BOOST_CHECK( normaliseFilename( "abc_def.txt" ) == "abc_def.txt" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
