// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Files::BatchLoadInfo.
 **/

#include <arinc_665/files/BatchLoadInfo.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <string_view>

namespace Arinc665::Files {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( BatchLoadInfoTest )

//! ARINC 665 BatchLoadInfo Constructor Test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  BatchLoadInfo batchLoadInfo{ "Filename1", "Partnumber1" };
  BOOST_CHECK( batchLoadInfo.headerFilename == "Filename1" );
  BOOST_CHECK( batchLoadInfo.partNumber == "Partnumber1" );
}

//! ARINC 665 Batch File Header Filename Test
BOOST_AUTO_TEST_CASE( headerFilename )
{
  using namespace std::string_view_literals;
  using namespace std::string_literals;

  BatchLoadInfo batchLoadInfo{};
  BOOST_CHECK( batchLoadInfo.headerFilename.empty() );
  BOOST_CHECK( batchLoadInfo.partNumber.empty() );

  batchLoadInfo.headerFilename = "Filename1";
  BOOST_CHECK( batchLoadInfo.headerFilename == "Filename1" );
  BOOST_CHECK( batchLoadInfo.partNumber.empty());
}

//! ARINC 665 Batch File Part Number Test
BOOST_AUTO_TEST_CASE( partNumber )
{
  using namespace std::string_view_literals;
  using namespace std::string_literals;

  BatchLoadInfo batchLoadInfo{};
  BOOST_CHECK( batchLoadInfo.headerFilename.empty() );
  BOOST_CHECK( batchLoadInfo.partNumber.empty() );

  batchLoadInfo.partNumber = "Partnumber1";
  BOOST_CHECK( batchLoadInfo.headerFilename.empty() );
  BOOST_CHECK( batchLoadInfo.partNumber == "Partnumber1" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
