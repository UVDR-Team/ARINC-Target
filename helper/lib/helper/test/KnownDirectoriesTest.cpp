// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Module KnownDirectories.
 **/

#include <helper/KnownDirectories.hpp>

#include <boost/test/unit_test.hpp>

namespace Helper {

BOOST_AUTO_TEST_SUITE( HelperTest )
BOOST_AUTO_TEST_SUITE( KnownDirectoriesTest )

//! KnownDirectories Test
BOOST_AUTO_TEST_CASE( KnownDirectoriesTest )
{
  BOOST_TEST_MESSAGE( "KnownDirectories_home " << KnownDirectories_home() );
  BOOST_TEST_MESSAGE( "KnownDirectories_dataHome " << KnownDirectories_dataHome() );
  BOOST_TEST_MESSAGE( "KnownDirectories_configHome " << KnownDirectories_configHome() );
  BOOST_TEST_MESSAGE( "KnownDirectories_stateHome " << KnownDirectories_stateHome() );
  BOOST_TEST_MESSAGE( "KnownDirectories_cacheHome " << KnownDirectories_cacheHome() );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
