// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Arinc665::Files::LoadFileInfo.
 **/

#include <arinc_665/files/LoadFileInfo.hpp>

#include <boost/test/unit_test.hpp>

using namespace std::string_view_literals;

namespace Arinc665::Files {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( FilesTest )
BOOST_AUTO_TEST_SUITE( LoadFileInfoTest )

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  LoadFileInfo loadFileInfo{ "NAME", "PART_NUMBER", 0xDEADBEEFUL, 0xBABEU, Arinc649::CheckValue::NoCheckValue };

  BOOST_CHECK( loadFileInfo.filename == "NAME" );
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER" );
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL );
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU );
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );
}

//! get/set name test
BOOST_AUTO_TEST_CASE( GetSetName )
{
  LoadFileInfo loadFileInfo{ "NAME", "PART_NUMBER", 0xDEADBEEFUL, 0xBABEU, Arinc649::CheckValue::NoCheckValue };

  BOOST_CHECK( loadFileInfo.filename == "NAME" );
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER" );
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL );
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU );
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );

  BOOST_CHECK_NO_THROW( loadFileInfo.filename = "NAME2" );

  BOOST_CHECK( loadFileInfo.filename == "NAME2" );
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER" );
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL );
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU );
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );
}

//! get/set part number test
BOOST_AUTO_TEST_CASE( GetSetPartNumber )
{
  LoadFileInfo loadFileInfo{ "NAME", "PART_NUMBER", 0xDEADBEEFUL, 0xBABEU, Arinc649::CheckValue::NoCheckValue };

  BOOST_CHECK( loadFileInfo.filename == "NAME" );
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER" );
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL );
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU );
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );

  BOOST_CHECK_NO_THROW( loadFileInfo.partNumber = "PART_NUMBER2" );

  BOOST_CHECK( loadFileInfo.filename == "NAME" );
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER2" );
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL );
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU );
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );
}

//! get/set length test
BOOST_AUTO_TEST_CASE( GetSetLength )
{
  LoadFileInfo loadFileInfo{ "NAME", "PART_NUMBER", 0xDEADBEEFUL, 0xBABEU, Arinc649::CheckValue::NoCheckValue };

  BOOST_CHECK( loadFileInfo.filename == "NAME");
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER");
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL);
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU);
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );

  BOOST_CHECK_NO_THROW( loadFileInfo.length = 0xAA55AA55UL );

  BOOST_CHECK( loadFileInfo.filename == "NAME");
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER");
  BOOST_CHECK( loadFileInfo.length == 0xAA55AA55UL);
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU);
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );
}

//! get/set crc test
BOOST_AUTO_TEST_CASE( GetSetCrc)
{
  LoadFileInfo loadFileInfo{ "NAME", "PART_NUMBER", 0xDEADBEEFUL, 0xBABEU, Arinc649::CheckValue::NoCheckValue };

  BOOST_CHECK( loadFileInfo.filename == "NAME");
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER");
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL);
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU);
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );

  BOOST_CHECK_NO_THROW( loadFileInfo.crc = 0xAA55U );

  BOOST_CHECK( loadFileInfo.filename == "NAME");
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER");
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL);
  BOOST_CHECK( loadFileInfo.crc == 0xAA55U);
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );
}

//! get/set check value test
BOOST_AUTO_TEST_CASE( GetSetCheckValue)
{
  LoadFileInfo loadFileInfo{ "NAME", "PART_NUMBER", 0xDEADBEEFUL, 0xBABEU, Arinc649::CheckValue::NoCheckValue };

  BOOST_CHECK( loadFileInfo.filename == "NAME" );
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER" );
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL );
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU );
  BOOST_CHECK( loadFileInfo.checkValue == Arinc649::CheckValue::NoCheckValue );

  BOOST_CHECK_NO_THROW(
    ( loadFileInfo.checkValue = Arinc649::CheckValue{
        Arinc649::CheckValueType::Crc8,
        std::vector< std::byte >{ std::byte{ 0x12 }, std::byte{ 0x34 } } } ) );

  BOOST_CHECK( loadFileInfo.filename == "NAME");
  BOOST_CHECK( loadFileInfo.partNumber == "PART_NUMBER");
  BOOST_CHECK( loadFileInfo.length == 0xDEADBEEFUL);
  BOOST_CHECK( loadFileInfo.crc == 0xBABEU);
  BOOST_CHECK(
    ( loadFileInfo.checkValue
      == Arinc649::CheckValue{
        Arinc649::CheckValueType::Crc8,
        std::vector< std::byte >{ std::byte{ 0x12 }, std::byte{ 0x34 } } } ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
