// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests for the class Arinc665::PartNumber.
 **/

#include <arinc_665/PartNumber.hpp>
#include <arinc_665/Arinc665Exception.hpp>

#include <boost/test/unit_test.hpp>

using namespace std::string_view_literals;

namespace Arinc665 {

BOOST_AUTO_TEST_SUITE( Arinc665Test )
BOOST_AUTO_TEST_SUITE( PartNumberTest )

//! Part Number Test
BOOST_AUTO_TEST_CASE( testPartNumber1 )
{
  PartNumber partNumber{ "PN1", "ABCDEFGH" };

  BOOST_CHECK( partNumber.manufacturerCode()  == "PN1" );
  BOOST_CHECK( partNumber.productIdentifier() == "ABCDEFGH" );

  BOOST_CHECK( partNumber.toString() ==
    std::string{ partNumber.manufacturerCode() } +
    std::string{ partNumber.checkCode() } +
    std::string{ partNumber.productIdentifier() } );

  BOOST_CHECK_THROW( (PartNumber{ "", "ABCDEFGH"} ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "A", "ABCDEFGH"} ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "AA", "ABCDEFGH"} ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "AAAA", "ABCDEFGH"} ), Arinc665Exception );


  BOOST_CHECK_THROW( (PartNumber{ "PN1", "" } ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "PN1", "1" } ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "PN1", "12" } ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "PN1", "123" } ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "PN1", "1234" } ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "PN1", "12345" } ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "PN1", "123456" } ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "PN1", "1234567" } ), Arinc665Exception );
  BOOST_CHECK_THROW( (PartNumber{ "PN1", "123456789" } ), Arinc665Exception );

}

//! Part Number Test
BOOST_AUTO_TEST_CASE( testPartNumber2 )
{
  PartNumber partNumber{ "PN127ABCDEFGH" };

  BOOST_CHECK( partNumber.manufacturerCode() == "PN1" );
  BOOST_CHECK( partNumber.productIdentifier() == "ABCDEFGH" );

  BOOST_CHECK( partNumber.toString() == "PN127ABCDEFGH" );

  BOOST_CHECK_THROW( (PartNumber{ "" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "0" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "01" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "012" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "0123" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "01234" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "012345" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "0123456" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "01234567" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "012345678" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "0123456789" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "0123456789A" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "0123456789AB" } ), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "0123456789ABC" } ), Arinc665Exception);
//  BOOST_CHECK_THROW( (PartNumber{ "0123456789ABCD"}), Arinc665Exception);
  BOOST_CHECK_THROW( (PartNumber{ "0123456789ABCDE" } ), Arinc665Exception);
}

//! ARINC 665 Manufacturer Code Tests
BOOST_AUTO_TEST_CASE( manufacturerCodeTest)
{
  PartNumber partNumber{ "AAA", "12345678" };
  BOOST_CHECK( partNumber.manufacturerCode() == "AAA");

  BOOST_CHECK_THROW( partNumber.manufacturerCode( ""sv), Arinc665Exception);
  BOOST_CHECK( partNumber.manufacturerCode() == "AAA");

  BOOST_CHECK_THROW( partNumber.manufacturerCode( "B"sv), Arinc665Exception);
  BOOST_CHECK( partNumber.manufacturerCode() == "AAA");

  BOOST_CHECK_THROW( partNumber.manufacturerCode( "BB"sv), Arinc665Exception);
  BOOST_CHECK( partNumber.manufacturerCode() == "AAA");

  BOOST_CHECK_NO_THROW( partNumber.manufacturerCode( "BBB"sv));
  BOOST_CHECK( partNumber.manufacturerCode() == "BBB");

  BOOST_CHECK_THROW( partNumber.manufacturerCode( "CCCC"sv), Arinc665Exception);
  BOOST_CHECK( partNumber.manufacturerCode() == "BBB");
}

//! ARINC 665 Product Identifier Test
BOOST_AUTO_TEST_CASE( productIdentifierTest)
{
  PartNumber partNumber{ "ABC", "12345678" };

  BOOST_CHECK( partNumber.productIdentifier() == "12345678" );

  BOOST_CHECK_THROW( partNumber.productIdentifier( ""sv), Arinc665Exception );
  BOOST_CHECK( partNumber.productIdentifier() == "12345678" );

  BOOST_CHECK_THROW( partNumber.productIdentifier( "1"sv ), Arinc665Exception );
  BOOST_CHECK( partNumber.productIdentifier() == "12345678" );

  BOOST_CHECK_THROW( partNumber.productIdentifier( "12"sv ), Arinc665Exception );
  BOOST_CHECK( partNumber.productIdentifier() == "12345678" );

  BOOST_CHECK_THROW( partNumber.productIdentifier( "123"sv ), Arinc665Exception );
  BOOST_CHECK( partNumber.productIdentifier() == "12345678" );

  BOOST_CHECK_THROW( partNumber.productIdentifier( "12345"sv ), Arinc665Exception );
  BOOST_CHECK( partNumber.productIdentifier() == "12345678" );

  BOOST_CHECK_THROW( partNumber.productIdentifier( "123456"sv ), Arinc665Exception );
  BOOST_CHECK( partNumber.productIdentifier() == "12345678" );

  BOOST_CHECK_THROW( partNumber.productIdentifier( "1234567"sv ), Arinc665Exception );
  BOOST_CHECK( partNumber.productIdentifier() == "12345678" );

  BOOST_CHECK_NO_THROW( partNumber.productIdentifier( "1234567A"sv ) );
  BOOST_CHECK( partNumber.productIdentifier() == "1234567A" );

  BOOST_CHECK_THROW( partNumber.productIdentifier( "123456789"sv ), Arinc665Exception );
  BOOST_CHECK( partNumber.productIdentifier() == "1234567A" );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
