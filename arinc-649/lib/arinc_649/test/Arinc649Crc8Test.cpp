// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for the ARINC 649 CRC-8.
 **/

#include <arinc_649/Arinc649Crc.hpp>

#include <arinc_649/test/Arinc649CrcTest.hpp>
#include <arinc_649/test/Arinc649TestData.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

#include <vector>

namespace Arinc649 {

BOOST_AUTO_TEST_SUITE( Arinc649Test )
BOOST_AUTO_TEST_SUITE( Arinc649Crc8Test )

//! CRC-8 test
BOOST_AUTO_TEST_CASE( crc8 )
{
  CheckCrc< Arinc649Crc8 >( getTestData( TestData::t01a ), getTestDataCrc8 ( TestData::t01a ) );
  CheckCrc< Arinc649Crc8 >( getTestData( TestData::t02a ), getTestDataCrc8 ( TestData::t02a ) );
  CheckCrc< Arinc649Crc8 >( getTestData( TestData::t03a ), getTestDataCrc8 ( TestData::t03a ) );
  CheckCrc< Arinc649Crc8 >( getTestData( TestData::t11a ), getTestDataCrc8 ( TestData::t11a ) );
  CheckCrc< Arinc649Crc8 >( getTestData( TestData::t12a ), getTestDataCrc8 ( TestData::t12a ) );
  CheckCrc< Arinc649Crc8 >( getTestData( TestData::t13a ), getTestDataCrc8 ( TestData::t13a ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
