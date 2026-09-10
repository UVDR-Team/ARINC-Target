// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for the ARINC 645 CRC-16.
 **/

#include <arinc_checksum/Arinc645Crc.hpp>

#include <arinc_checksum/test/Arinc645CrcTest.hpp>
#include <arinc_checksum/test/Arinc645TestData.hpp>

#include <arinc_support/RawData.hpp>

#include <boost/test/unit_test.hpp>

#include <vector>

namespace ArincChecksum {

BOOST_AUTO_TEST_SUITE( Arinc645Test )
BOOST_AUTO_TEST_SUITE( Arinc645Crc16Test )

//! CRC-16 test
BOOST_AUTO_TEST_CASE( crc16 )
{
  CheckCrc< Arinc645Crc16 >( getTestData( TestData::t01a ), getTestDataCrc16 ( TestData::t01a ) );
  CheckCrc< Arinc645Crc16 >( getTestData( TestData::t02a ), getTestDataCrc16 ( TestData::t02a ) );
  CheckCrc< Arinc645Crc16 >( getTestData( TestData::t03a ), getTestDataCrc16 ( TestData::t03a ) );
  CheckCrc< Arinc645Crc16 >( getTestData( TestData::t04a ), getTestDataCrc16 ( TestData::t04a ) );
  CheckCrc< Arinc645Crc16 >( getTestData( TestData::t11a ), getTestDataCrc16 ( TestData::t11a ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
