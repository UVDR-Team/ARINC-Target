// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for the class Arinc665::Files::BatchFile.
 **/

#include <arinc_665/files/BatchFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <array>

namespace Arinc665::Files {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( BatchFileTest )

//! Raw Batch File
static const uint8_t rawBatchFile[]{
  // header file length
  0x00, 0x00, 0x00, 0x20,
  // Format version
  0x90, 0x03,
  // spare
  0x00, 0x00,
  // Pointer to Batch Part Number
  0x00, 0x00, 0x00, 0x08,
  // Pointer to Target HW ID list
  0x00, 0x00, 0x00, 0x11,
  // Expansion Point No.1

  /* 16 */
  // Batch PN Length
  0x00, 0x05,
  // Load PN
  'P', 'N', '1', '2', '3', 0x00,
  // Comment Length
  0x00, 0x07,
  // Comment
  'C', 'O', 'M', 'M', 'E', 'N', 'T', 0x00,

  /* 34 */
  // Number of Target HW IDs
  0x00, 0x01,

  // Pointer to Next THW ID
  0x00, 0x00,
  // Target HW ID POS Length
  0x00, 0x04,
  // Target HW ID POS
  'T', 'H', 'W', '0',

  // Number Of Loads
  0x00, 0x01,

  // Header File Name Length
  0x00, 0x06,
  // Header File Name
  'F', 'N', '_', '0', '0', '1',
  // Load PN Length
  0x00, 0x06,
  // Load PN
  'P', 'N', '_', '0', '0', '1',
  // Expansion Point No. 3


  /* 62 */

  // Batch File CRC
  0x76, 0x48,
  /* 64 */
};

BOOST_AUTO_TEST_CASE( constructor1 )
{
  BatchFile file{ SupportedArinc665Version::Supplement2 };

  BOOST_CHECK( file.arincVersion() == SupportedArinc665Version::Supplement2 );

  BOOST_CHECK( file.partNumber().empty() );
  BOOST_CHECK( file.comment().empty() );

  BOOST_CHECK( file.targetsHardware().empty() );
}

//! Constructor Test - Raw Decoding
BOOST_AUTO_TEST_CASE( constructor2 )
{
  BatchFile file{ std::as_bytes( std::span{ rawBatchFile } ) };

  BOOST_CHECK( file.arincVersion() == SupportedArinc665Version::Supplement2 );

  BOOST_CHECK( file.partNumber() == "PN123" );
  BOOST_CHECK( file.comment() == "COMMENT" );

  const auto &targetHardwareIds{ file.targetsHardware() };
  BOOST_CHECK( targetHardwareIds.size() == 1U );
  BOOST_CHECK( targetHardwareIds.begin()->targetHardwareIdPosition == "THW0" );
  const auto &loads{ targetHardwareIds.begin()->loads };
  BOOST_CHECK( loads.size() == 1U );
  BOOST_CHECK( loads.begin()->headerFilename == "FN_001" );
  BOOST_CHECK( loads.begin()->partNumber == "PN_001" );

  const auto raw2{ static_cast< Helper::RawData >( file ) };

  BOOST_CHECK( std::ranges::equal( std::as_bytes( std::span{ rawBatchFile } ), raw2 ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
