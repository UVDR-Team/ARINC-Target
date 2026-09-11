// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Files::BatchListFile.
 **/

#include <arinc_665/files/BatchListFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <array>

namespace Arinc665::Files {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( BatchListFileTest )

//! Raw List of Batches File
static const uint8_t rawBatchListFile[]{
  // header file length
  0x00, 0x00, 0x00, 0x28,
  // Format version
  0xA0, 0x03,
  // spare
  0x00, 0x00,
  // Pointer to Media Information
  0x00, 0x00, 0x00, 0x0A,
  // Pointer to Number of Batches
  0x00, 0x00, 0x00, 0x0F,
  // Pointer to User Defined Data
  0x00, 0x00, 0x00, 0x24,
  // Expansion Point No.1

  /* 20 */
  // Media Set PN Length
  0x00, 0x05,
  // Media Set PN
  'P', 'N', '1', '2', '3', 0x00,
  // Media Sequence Number
  0x01,
  // Number of media set members
  0x02,

  /* 30 */
  // Number of Batches
  0x00, 0x02,

  /* 32 */
  // Batch pointer
  0x00, 0x0A,
  // Batch PN length
  0x00, 0x05,
  // Batch PN
  'P', 'N', '0', '0', '1', 0x00,
  // Batch Name Length
  0x00, 0x06,
  // Batch Name
  'F', 'N', '_', '0', '0', '1',
  // Member Sequence Number
  0x00, 0x01,
  // Expansion Point No. 2

  /* 52 */
  // Batch pointer
  0x00, 0x00,
  // Batch PN length
  0x00, 0x05,
  // Batch PN
  'P', 'N', '0', '0', '2', 0x00,
  // Batch File Name Length
  0x00, 0x06,
  // Batch File Name
  'F', 'N', '_', '0', '0', '2',
  // Member Sequence Number
  0x00, 0x01,
  // Expansion Point No. 2

  // Expansion Point No. 3

  /* 72 */
  // User Defined Data
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,

  // File CRC
  0x36, 0x95 };

BOOST_AUTO_TEST_CASE( constructor1 )
{
  BatchListFile file{ std::as_bytes( std::span{ rawBatchListFile } ) };

  BOOST_CHECK( file.arincVersion() == SupportedArinc665Version::Supplement2 );

  BOOST_CHECK( file.mediaSetPn() == "PN123" );
  BOOST_CHECK( file.mediaSequenceNumber() == MediumNumber{ 1U } );
  BOOST_CHECK( file.numberOfMediaSetMembers() == MediumNumber{ 2U } );

  const auto& batches{ file.batches() };
  BOOST_CHECK( batches.size() == 2 );

  auto batch{ batches.begin() };
  BOOST_CHECK( batch->partNumber == "PN001" );
  BOOST_CHECK( batch->filename == "FN_001" );
  BOOST_CHECK( batch->memberSequenceNumber == MediumNumber{ 1U } );

  ++batch;
  BOOST_CHECK( batch->partNumber == "PN002" );
  BOOST_CHECK( batch->filename == "FN_002" );
  BOOST_CHECK( batch->memberSequenceNumber == MediumNumber{ 1U } );

  BOOST_CHECK( file.userDefinedData().size() == 6 );

  const uint8_t expected[]{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
  BOOST_CHECK( std::ranges::equal( file.userDefinedData(), std::as_bytes( std::span{ expected } ) ) );

  const auto raw2{ static_cast< Helper::RawData >( file ) };
  BOOST_CHECK( std::ranges::equal( std::as_bytes( std::span{ rawBatchListFile } ), raw2 ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
