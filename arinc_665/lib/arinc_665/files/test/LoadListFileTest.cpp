// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Files::LoadListFile.
 **/

#include <arinc_665/files/LoadListFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <boost/test/unit_test.hpp>

#include <array>

namespace Arinc665::Files {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( LoadListFileTest )

//! Raw List of Loads File
static const uint8_t rawLoadListFile[]{
  // header file length
  0x00, 0x00, 0x00, 0x34,
  // Format version
  0xA0, 0x03,
  // spare
  0x00, 0x00,
  // Pointer to Media Information
  0x00, 0x00, 0x00, 0x0A,
  // Pointer to Load List
  0x00, 0x00, 0x00, 0x0F,
  // Pointer to User Defined Data
  0x00, 0x00, 0x00, 0x30,
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
  // Number of loads
  0x00, 0x02,

  /* 32 */
  // load pointer
  0x00, 0x10,
  // Load PN length
  0x00, 0x05,
  // Load PN
  'P', 'N', '0', '0', '1', 0x00,
  // Header File Name Length
  0x00, 0x06,
  // Header File Name
  'F', 'N', '_', '0', '0', '1',
  // Member Sequence Number
  0x00, 0x01,
  // Number of THW IDs
  0x00, 0x01,
  // THW ID Length
  0x00, 0x08,
  'T', 'H', 'W', 'I', 'D', '0', '1', '0',
  // Expansion Point No. 2

  /* 64 */
  // load pointer
  0x00, 0x00,
  // Load PN length
  0x00, 0x05,
  // Load PN
  'P', 'N', '0', '0', '2', 0x00,
  // Header File Name Length
  0x00, 0x06,
  // Header File Name
  'F', 'N', '_', '0', '0', '2',
  // Member Sequence Number
  0x00, 0x01,
  // Number of THW IDs
  0x00, 0x01,
  // THW ID Length
  0x00, 0x08,
  'T', 'H', 'W', 'I', 'D', '0', '1', '0',
  // Expansion Point No. 2

  // Expansion Point No. 3

  /* 96 */
  // User Defined Data
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,

  // File CRC
  0xCE, 0x32
};

BOOST_AUTO_TEST_CASE( constructor1 )
{
  LoadListFile file{ std::as_bytes( std::span{ rawLoadListFile } ) };

  BOOST_CHECK( file.arincVersion() == SupportedArinc665Version::Supplement2 );

  BOOST_CHECK( file.mediaSetPn() == "PN123" );
  BOOST_CHECK( file.mediaSequenceNumber() == MediumNumber{ 1U } );
  BOOST_CHECK( file.numberOfMediaSetMembers() == MediumNumber{ 1U } );

  const auto& loads{ file.loads() };
  BOOST_CHECK( loads.size() == 2 );

  auto load{ loads.begin() };
  BOOST_CHECK( load->partNumber == "PN001" );
  BOOST_CHECK( load->headerFilename == "FN_001" );
  BOOST_CHECK( load->memberSequenceNumber == MediumNumber{ 1U } );
  BOOST_CHECK( load->targetHardwareIds.size() == 1 );
  BOOST_CHECK( *load->targetHardwareIds.begin() == "THWID010" );

  ++load;
  BOOST_CHECK( load->partNumber == "PN002" );
  BOOST_CHECK( load->headerFilename == "FN_002" );
  BOOST_CHECK( load->memberSequenceNumber == MediumNumber{ 1U } );
  BOOST_CHECK( load->targetHardwareIds.size() == 1U );
  BOOST_CHECK( *load->targetHardwareIds.begin() == "THWID010" );

  BOOST_CHECK( file.userDefinedData().size() == 6 );

  const uint8_t expected[]{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
  BOOST_CHECK( std::ranges::equal( file.userDefinedData(), std::as_bytes( std::span{ expected } ) ) );

  const auto raw2{ static_cast< Helper::RawData >( file ) };
  BOOST_CHECK( std::ranges::equal( std::as_bytes( std::span{ rawLoadListFile } ), raw2 ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
