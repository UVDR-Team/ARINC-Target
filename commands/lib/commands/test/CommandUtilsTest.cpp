// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Commands CommandUtils.
 **/

#include <commands/CommandRegistry.hpp>
#include <commands/Utils.hpp>

#include <boost/test/unit_test.hpp>

namespace Commands {

BOOST_AUTO_TEST_SUITE( CommandsTest )
BOOST_AUTO_TEST_SUITE( UtilsTest )

//! Version Test
BOOST_AUTO_TEST_CASE( commandRegistry )
{
  const auto registry{ CommandRegistry::instance() };

  BOOST_CHECK( registry );

  Utils_alias( *registry, "echo1", "echo" );
  Utils_echoCommand( *registry );
  Utils_echoCommand( *registry );
  Utils_alias( *registry, "echo1", "echo" );
  Utils_alias( *registry, "echo1", "echo" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
