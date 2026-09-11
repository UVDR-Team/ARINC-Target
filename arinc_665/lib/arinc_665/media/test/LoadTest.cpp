// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Media::Load.
 **/

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <boost/test/unit_test.hpp>

using namespace std::string_view_literals;

namespace Arinc665::Media {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( MediaTest )
BOOST_AUTO_TEST_SUITE( LoadTest )

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor )
{
  using namespace std::string_view_literals;

  auto mediaSet{ MediaSet::create() };

  auto load{ mediaSet->addLoad( "LOAD.LUH" ) };
  BOOST_CHECK( load );
  BOOST_CHECK( load->name() == "LOAD.LUH" );
  BOOST_CHECK( load->fileType() == FileType::LoadFile );
  BOOST_CHECK( !load->mediumNumber() );
  BOOST_CHECK( load->effectiveMediumNumber() == MediumNumber{ 1U } );

  BOOST_CHECK( load->partNumber().empty() );
  load->partNumber( "YYY" );
  BOOST_CHECK( load->partNumber() == "YYY" );


  auto regularFile1{ mediaSet->addRegularFile( "FILE1" ) };
  auto regularFile2{ mediaSet->addRegularFile( "FILE2" ) };
  auto regularFile3{ mediaSet->addRegularFile( "FILE3" ) };
  auto regularFile4{ mediaSet->addRegularFile( "FILE4" ) };

  load->dataFile( regularFile1, "PN1" );
  load->dataFile( regularFile2, "PN2" );
  load->supportFile( regularFile3, "PN3" );
  load->supportFile( regularFile4, "PN4" );

  BOOST_CHECK( load->file( "LOAD.LUH" ) == load );
  BOOST_CHECK( load->file( "FILE1" ) == regularFile1 );
  BOOST_CHECK( load->file( "FILE2" ) == regularFile2 );
  BOOST_CHECK( load->file( "FILE3" ) == regularFile3 );
  BOOST_CHECK( load->file( "FILE4" ) == regularFile4 );
  BOOST_CHECK( !load->file( "INVALID" ) );

  BOOST_CHECK( load->userDefinedData().empty() );
}

//! Load by Part Number Test
BOOST_AUTO_TEST_CASE( Loads_loadByPartNumberTest )
{
  using namespace std::string_view_literals;

  auto mediaSet{ MediaSet::create() };

  auto load1{ mediaSet->addLoad( "LOAD1.LUH" ) };
  BOOST_CHECK( load1 );
  load1->partNumber( "PN1" );
  BOOST_CHECK( load1->partNumber() == "PN1" );

  auto load2{ mediaSet->addLoad( "LOAD2.LUH" ) };
  BOOST_CHECK( load2 );
  load2->partNumber( "PN2" );
  BOOST_CHECK( load2->partNumber() == "PN2" );

  ConstLoads loads{ load1, load2 };

  BOOST_CHECK( !Loads_loadByPartNumber( loads, "XXX" ) );
  BOOST_CHECK( Loads_loadByPartNumber( loads, "PN1" ) == load1 );
  BOOST_CHECK( Loads_loadByPartNumber( loads, "PN2" ) == load2 );
}

//! File by Part Number Test
BOOST_AUTO_TEST_CASE( Loads_filePartNumberTest )
{
  using namespace std::string_view_literals;

  auto mediaSet{ MediaSet::create() };

  auto load1{ mediaSet->addLoad( "LOAD1.LUH" ) };
  BOOST_CHECK( load1 );
  load1->partNumber( "PN1" );
  BOOST_CHECK( load1->partNumber() == "PN1" );

  auto load2{ mediaSet->addLoad( "LOAD2.LUH" ) };
  BOOST_CHECK( load2 );
  load2->partNumber( "PN2" );
  BOOST_CHECK( load2->partNumber() == "PN2" );

  ConstLoads loads{ load1, load2 };

  BOOST_CHECK( !Loads_file( loads, "XXX" ) );
  BOOST_CHECK( Loads_file( loads, "LOAD1.LUH", "PN1" ) == load1 );
  BOOST_CHECK( Loads_file( loads, "LOAD2.LUH", "PN2" ) == load2 );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
