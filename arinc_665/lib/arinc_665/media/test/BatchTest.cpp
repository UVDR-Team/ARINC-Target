// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Media::Batch.
 **/

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/Batch.hpp>
#include <arinc_665/media/Load.hpp>

#include <boost/test/unit_test.hpp>

using namespace std::string_view_literals;

namespace Arinc665::Media {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( MediaTest )
BOOST_AUTO_TEST_SUITE( BacthTest )

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor )
{
  using namespace std::string_view_literals;

  auto mediaSet{ MediaSet::create() };

  auto batch{ mediaSet->addBatch( "BATCH.LUB" ) };
  BOOST_CHECK( batch );
  BOOST_CHECK( batch->name() == "BATCH.LUB" );
  BOOST_CHECK( batch->fileType() == FileType::BatchFile );
  BOOST_CHECK( !batch->mediumNumber() );
  BOOST_CHECK( batch->effectiveMediumNumber() == MediumNumber{ 1U } );

  BOOST_CHECK( batch->partNumber().empty() );
  batch->partNumber( "YYY" );
  BOOST_CHECK( batch->partNumber() == "YYY" );


  auto load1{ mediaSet->addLoad( "LOAD1.LUH" ) };
  auto load2{ mediaSet->addLoad( "LOAD2.LUH" ) };
  auto load3{ mediaSet->addLoad( "LOAD3.LUH" ) };
  auto load4{ mediaSet->addLoad( "LOAD4.LUH" ) };

  batch->target( "THWID1_POS1", { load1, load2 } );
  batch->target( "THWID2_POS2", { load3, load4 } );

  BOOST_CHECK( batch->target( "XXX_YYY" ).empty() );
  BOOST_CHECK( ( batch->target( "THWID1_POS1" ) == ConstLoads{ load1, load2 } ) );
  BOOST_CHECK( ( batch->target( "THWID2_POS2" ) == ConstLoads{ load3, load4 } ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
