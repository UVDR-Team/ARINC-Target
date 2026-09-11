// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Media::MediaSet.
 **/

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/Batch.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <boost/test/unit_test.hpp>

using namespace std::string_view_literals;

namespace Arinc665::Media {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( MediaTest )
BOOST_AUTO_TEST_SUITE( MediaSetTest )

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor )
{
  auto mediaSet{ MediaSet::create() };

  BOOST_CHECK( mediaSet->partNumber().empty() );
  BOOST_CHECK( mediaSet->lastMediumNumber() == MediumNumber{ 1U } );
  BOOST_CHECK( mediaSet->type() == Type::MediaSet );

  BOOST_CHECK( mediaSet->numberOfFiles() == 0 );
  BOOST_CHECK( mediaSet->files().empty() );

  BOOST_CHECK( mediaSet->numberOfLoads() == 0 );
  BOOST_CHECK( mediaSet->loads().empty() );

  BOOST_CHECK( mediaSet->numberOfBatches() == 0 );
  BOOST_CHECK( mediaSet->batches().empty() );
}

//! part number test
BOOST_AUTO_TEST_CASE( partNumber )
{
  auto mediaSet{ MediaSet::create() };

  BOOST_CHECK( mediaSet->partNumber().empty() );
  mediaSet->partNumber( "YYY" );
  BOOST_CHECK( mediaSet->partNumber() == "YYY" );
}

//! medium test
BOOST_AUTO_TEST_CASE( medium )
{
  auto mediaSet{ MediaSet::create() };
  BOOST_CHECK( mediaSet );
  BOOST_CHECK( mediaSet->lastMediumNumber() == MediumNumber{ 1U } );

  BOOST_CHECK( mediaSet->addRegularFile( "Medium1File", Arinc665::MediumNumber{ 1U } ) );
  BOOST_CHECK( mediaSet->lastMediumNumber() == MediumNumber{ 1U } );

  BOOST_CHECK( mediaSet->addRegularFile( "Medium2File", Arinc665::MediumNumber{ 2U } ) );
  BOOST_CHECK( mediaSet->lastMediumNumber() == MediumNumber{ 2U } );

  BOOST_CHECK( mediaSet->addRegularFile( "Medium2aFile", Arinc665::MediumNumber{ 2U } ) );
  BOOST_CHECK( mediaSet->lastMediumNumber() == MediumNumber{ 2U } );

  BOOST_CHECK( mediaSet->addRegularFile( "Medium6File", Arinc665::MediumNumber{ 6U } ) );
  BOOST_CHECK( mediaSet->lastMediumNumber() == MediumNumber{ 6U } );

  BOOST_CHECK_NO_THROW( mediaSet->removeFile( "Medium6File") );
  BOOST_CHECK( mediaSet->lastMediumNumber() == MediumNumber{ 2U } );
}

//! Regular Files test
BOOST_AUTO_TEST_CASE( regularFiles )
{
  using namespace std::string_view_literals;

  auto mediaSet{ MediaSet::create() };

  BOOST_CHECK( !mediaSet->regularFile( "FILE1"sv ) );
  auto regularFile{ mediaSet->addRegularFile( "FILE1" ) };
  BOOST_CHECK( regularFile );

  BOOST_CHECK( mediaSet->file( "FILE1"sv ) == regularFile );

  BOOST_CHECK( mediaSet->regularFile( "FILE1"sv ) == regularFile );
  BOOST_CHECK( mediaSet->regularFile( std::filesystem::path{ "/FILE1" } ) == regularFile );

  BOOST_CHECK( !mediaSet->batch( "FILE1"sv ) );
  BOOST_CHECK( !mediaSet->batch( std::filesystem::path{ "/FILE1" } ) );

  BOOST_CHECK( !mediaSet->load( "LOAD1.LUH"sv ) );
  BOOST_CHECK( !mediaSet->load( std::filesystem::path{ "/FILE1" } ) );
}

//! Loads test
BOOST_AUTO_TEST_CASE( loads )
{
  using namespace std::string_view_literals;

  auto mediaSet{ MediaSet::create() };

  BOOST_CHECK( !mediaSet->load( "LOAD1.LUH"sv ) );
  auto load1{ mediaSet->addLoad( "LOAD1.LUH" ) };
  BOOST_CHECK( load1 );

  BOOST_CHECK( mediaSet->file( "LOAD1.LUH"sv ) == load1 );

  BOOST_CHECK( !mediaSet->regularFile( "LOAD1.LUH"sv ) );
  BOOST_CHECK( !mediaSet->regularFile( std::filesystem::path{ "/LOAD1.LUH" } ) );

  BOOST_CHECK( !mediaSet->batch( "LOAD1.LUH"sv ) );
  BOOST_CHECK( !mediaSet->batch( std::filesystem::path{ "/LOAD1.LUH" } ) );

  BOOST_CHECK( mediaSet->load( "LOAD1.LUH"sv ) == load1 );
  BOOST_CHECK( mediaSet->load( std::filesystem::path{ "/LOAD1.LUH" } ) == load1 );
}

//! Batches test
BOOST_AUTO_TEST_CASE( batches )
{
  using namespace std::string_view_literals;

  auto mediaSet{ MediaSet::create() };

  BOOST_CHECK( !mediaSet->batch( "BATCH1.LUB"sv ) );
  auto batch{ mediaSet->addBatch( "BATCH1.LUB" ) };
  BOOST_CHECK( batch );

  BOOST_CHECK( mediaSet->file( "BATCH1.LUB"sv ) == batch );

  BOOST_CHECK( !mediaSet->regularFile( "BATCH1.LUB"sv ) );
  BOOST_CHECK( !mediaSet->regularFile( std::filesystem::path{ "/BATCH1.LUB" } ) );

  BOOST_CHECK( !mediaSet->load( "BATCH1.LUB"sv ) );
  BOOST_CHECK( !mediaSet->load( std::filesystem::path{ "/BATCH1.LUB" } ) );

  BOOST_CHECK( mediaSet->batch( "BATCH1.LUB"sv ) == batch );
  BOOST_CHECK( mediaSet->batch( std::filesystem::path{ "/BATCH1.LUB" } ) == batch );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
