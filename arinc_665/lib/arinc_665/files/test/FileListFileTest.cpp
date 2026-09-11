// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Files::FileListFile.
 **/

#include <arinc_665/files/FileListFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <array>

namespace Arinc665::Files {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( FileListFileTest )

//! Raw List of Files File
static const uint8_t rawFileListFile[]{
  // header file length
  0x00, 0x00, 0x00, 0x27,
  // Format version
  0xA0, 0x03,
  // spare
  0x00, 0x00,
  // Pointer to Media Information
  0x00, 0x00, 0x00, 0x0A,
  // Pointer to Files Info
  0x00, 0x00, 0x00, 0x0F,
  // Pointer to User Defined Data
  0x00, 0x00, 0x00, 0x23,
  // Expansion Point No.1

  /* 20 */
  // Media Set PN Length
  0x00, 0x05,
  // Media Set PN
  'P', 'N', '1', '2', '3', 0x00,
  // Media Sequence Number
  0x01,
  // Number of media set members
  0x01,

  /* 30 */
  // Number of Files
  0x00, 0x02,

  /* 32 */
  // File pointer
  0x00, 0x09,
  // File Name length
  0x00, 0x06,
  // File Name
  'F', 'N', '_', '0', '0', '1',
  // File Path Length
  0x00, 0x01,
  // File Path
  '\\', 0x00,
  // Member Sequence Number
  0x00, 0x01,
  // File CRC
  0xAB, 0xCD,
  // Expansion Point No. 2

  /* 50 */
  // File pointer
  0x00, 0x00,
  // File Name length
  0x00, 0x06,
  // File Name
  'F', 'N', '_', '0', '0', '2',
  // File Path Length
  0x00, 0x03,
  // File Path Name
  '\\', 'A', '\\', 0x00,
  // Member Sequence Number
  0x00, 0x01,
  // File CRC
  0x01, 0x23,
  // Expansion Point No. 2

  // Expansion Point No. 3

  /* 70 */
  // User Defined Data
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,

  /* 76 */
  // File CRC
  0xCB, 0xF7 };

BOOST_AUTO_TEST_CASE( constructor1 )
{
  FileListFile file{ std::as_bytes( std::span{ rawFileListFile } ) };

  BOOST_CHECK( file.arincVersion() == SupportedArinc665Version::Supplement2 );

  BOOST_CHECK( file.mediaSetPn() == "PN123" );
  BOOST_CHECK( file.mediaSequenceNumber() == MediumNumber{ 1U } );
  BOOST_CHECK( file.numberOfMediaSetMembers() == MediumNumber{ 1U } );

  const auto& files{ file.files() };
  BOOST_CHECK( files.size() == 2 );

  auto fileI{ files.begin() };
  BOOST_CHECK( fileI->filename == "FN_001" );
  BOOST_CHECK( fileI->pathName == "\\" );
  BOOST_CHECK( fileI->path() == "/FN_001" );
  BOOST_CHECK( fileI->memberSequenceNumber == MediumNumber{ 1U } );
  BOOST_CHECK( fileI->crc == 0xABCDU );

  ++fileI;
  BOOST_CHECK( fileI->filename == "FN_002" );
  BOOST_CHECK( fileI->pathName == "\\A\\" );
  BOOST_CHECK( fileI->path() == "/A/FN_002" );
  BOOST_CHECK( fileI->memberSequenceNumber == MediumNumber{ 1U } );
  BOOST_CHECK( fileI->crc == 0x0123U );

  BOOST_CHECK( file.userDefinedData().size() == 6 );

  const uint8_t expected[]{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
  BOOST_CHECK( std::ranges::equal( file.userDefinedData(), std::as_bytes( std::span{ expected } ) ) );

  const auto raw2{ static_cast< Helper::RawData >( file ) };
  BOOST_CHECK( std::ranges::equal( std::as_bytes( std::span{ rawFileListFile } ), raw2 ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
