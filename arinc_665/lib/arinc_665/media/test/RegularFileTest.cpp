// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Media::RegularFile.
 **/

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <boost/test/unit_test.hpp>

using namespace std::string_view_literals;

namespace Arinc665::Media {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( MediaTest )
BOOST_AUTO_TEST_SUITE( RegularFileTest )

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor )
{
  using namespace std::string_view_literals;

  auto mediaSet{ MediaSet::create() };

  auto regularFile{ mediaSet->addRegularFile( "FILE1" ) };
  BOOST_CHECK( regularFile );
  BOOST_CHECK( regularFile->name() == "FILE1" );
  BOOST_CHECK( regularFile->fileType() == FileType::RegularFile );
  BOOST_CHECK( !regularFile->mediumNumber() );
  BOOST_CHECK( regularFile->effectiveMediumNumber() == MediumNumber{ 1U } );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
