// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Unit test of Check Value Utilities.
 **/

#include <arinc_665/files/CheckValueUtils.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_checksum/CheckValue.hpp>

#include <arinc_support/RawData.hpp>
#include <arinc_support/Dump.hpp>

#include <boost/test/unit_test.hpp>

namespace Arinc665::Files {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( CheckValueUtilsTest )

//! CheckValueUtils_encode Test
BOOST_AUTO_TEST_CASE( CheckValueUtils_encode1 )
{
  using ArincSupport::operator""_b;

  BOOST_CHECK( CheckValueUtils_encode( ArincChecksum::CheckValue::NoCheckValue ) == ArincSupport::RawData( { 0x00_b, 0x00_b } ) );
  BOOST_CHECK( (
    CheckValueUtils_encode( ArincChecksum::CheckValue{ ArincChecksum::CheckValueType::Crc8, ArincSupport::RawData{ 0x12_b, 0x34_b } } )
      == ArincSupport::RawData{ 0x00_b, 0x06_b, 0x00_b, 0x01_b, 0x12_b, 0x34_b } ) );
}

//! CheckValueUtils_decode Test
BOOST_AUTO_TEST_CASE( CheckValueUtils_decode1 )
{
  using ArincSupport::operator""_b;

  const auto cv1a{ CheckValueUtils_decode( ArincSupport::RawData{ 0x00_b, 0x00_b } ) };

  BOOST_CHECK( ( cv1a == ArincChecksum::CheckValue{ ArincChecksum::CheckValue::NoCheckValue } ) );

  const auto cv1b{ CheckValueUtils_decode( ArincSupport::RawData{ 0x00_b, 0x04_b, 0x00_b, 0x00_b } ) };

  BOOST_CHECK( ( cv1b == ArincChecksum::CheckValue{ ArincChecksum::CheckValue::NoCheckValue } ) );

  const auto cv2{ CheckValueUtils_decode( ArincSupport::RawData{ 0x00_b, 0x06_b, 0x00_b, 0x01_b, 0x12_b, 0x34_b } ) };

  BOOST_CHECK( ( cv2 == ArincChecksum::CheckValue{ ArincChecksum::CheckValueType::Crc8, ArincSupport::RawData{ 0x12_b, 0x34_b } } ) );

  const auto cv3{ CheckValueUtils_decode( ArincSupport::RawData{ 0x00_b, 0x06_b, 0x00_b, 0x02_b, 0x12_b, 0x34_b } ) };

  BOOST_CHECK( ( cv3 == ArincChecksum::CheckValue{ ArincChecksum::CheckValueType::Crc16, ArincSupport::RawData{ 0x12_b, 0x34_b } } ) );

  BOOST_CHECK_THROW( boost::ignore_unused( CheckValueUtils_decode( ArincSupport::RawData{} ) ), Arinc665Exception );

  BOOST_CHECK_THROW( boost::ignore_unused( CheckValueUtils_decode( ArincSupport::RawData{ 0x00_b } ) ), Arinc665Exception );

  BOOST_CHECK_THROW(
    boost::ignore_unused( CheckValueUtils_decode( ArincSupport::RawData{ 0x00_b, 0x05_b, 0x00_b, 0x01_b, 0x12_b } ) ),
    Arinc665Exception );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
