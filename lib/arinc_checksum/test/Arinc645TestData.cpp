// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc645TestData.
 **/

#include "Arinc645TestData.hpp"

#include <arinc_support/RawData.hpp>

#include <boost/endian.hpp>

#include <ranges>
#include <string_view>
#include <vector>

namespace ArincChecksum {

std::span< const std::byte > getTestData( const TestData testData )
{
  //! 128 Bytes of 0xFF
  static const std::vector Arinc645TestData_t02a( 128, std::byte{ 0xFF } );

  //! 100 Bytes of 0x00
  static const std::vector Arinc645TestData_t03a( 100, std::byte{ 0 } );

  //! 128 16bit-words of 0xAA55
  static const std::vector Arinc645TestData_t04a( 128, boost::endian::native_to_big< uint16_t >( 0xAA55 ) );

  //! 256 Bytes of 0x00 through 0xFF
  static const auto Arinc645TestData_t11a = [] {
    std::vector<std::uint8_t> bytes(256);
    for (std::size_t i = 0; i < bytes.size(); ++i) bytes[i] = static_cast<std::uint8_t>(i);
    return bytes;
  }();

  //! String "ACM12345678"
  static constexpr std::string_view Arinc645TestData_t12a{ "ACM12345678" };

  //! String "ABCDEFGHIJKLMNO"
  static constexpr std::string_view Arinc645TestData_t13a{ "ABCDEFGHIJKLMNO" };

  switch ( testData )
  {
    using enum TestData;

    //! Empty
    case t01a:
      return {};

    case t02a:
      return Arinc645TestData_t02a;

    case t03a:
      return Arinc645TestData_t03a;

    case t04a:
      return ArincSupport::RawData_asRawData( Arinc645TestData_t04a );

    case t11a:
      return ArincSupport::RawData_asRawData( Arinc645TestData_t11a );

    case t12a:
      return ArincSupport::RawData_asRawData( Arinc645TestData_t12a );

    case t13a:
      return ArincSupport::RawData_asRawData( Arinc645TestData_t13a );

    default:
      return {};
  }
}

uint8_t getTestDataCrc8( const TestData testData )
{
  switch ( testData )
  {
    using enum TestData;

    case t01a:
      return 0x00;

    case t02a:
      return 0x00;

    case t03a:
      return 0x00;

    case t04a:
      return 0x00; // not given

    case t11a:
      return 0x00; // taken from ARINC 645-1 Section 4.3.4

    case t12a:
      return 0x47;

    case t13a:
      return 0x40;

    default:
      return 0x00;
  }
}

uint16_t getTestDataCrc16( const TestData testData )
{
  switch ( testData )
  {
    using enum TestData;

    case t01a:
      return 0xFFFF;

    case t02a:
      return 0x1DA3;

    case t03a:
      return 0x4634;

    case t04a:
      return 0x1D7E;

    case t11a:
      return 0x3FBD;

    case t12a:
      return 0x0000; // not given

    case t13a:
      return 0x0000; // not given

    default:
      return 0x0000;
  }
}

uint32_t getTestDataCrc32( const TestData testData )
{
  switch ( testData )
  {
    using enum TestData;

    case t01a:
      return 0x0000'0000;

    case t02a:
      return 0x322A'B4A6;

    case t03a:
      return 0x5363'1199;

    case t04a:
      return 0xC2F2'70BC;

    case t11a:
      return 0xB6B5'EE95;

    case t12a:
      return 0x0000'0000; // not given

    case t13a:
      return 0x0000'0000; // not given

    default:
      return 0x00000000;
  }
}

uint64_t getTestDataCrc64( const TestData testData )
{
  switch ( testData )
  {
    using enum TestData;

    case t01a:
      return 0x0000'0000'0000'0000;

    case t02a:
      return 0x0345'28B5'989B'ED4D;

    case t03a:
      return 0x5B2A'CFD2'703E'D63D;

    case t04a:
      return 0x428A'028B'4742'33E4;

    case t11a:
      return 0x59C3'325B'2927'A19A;

    case t12a:
      return 0x0000'0000'0000'0000; // not given

    case t13a:
      return 0x0000'0000'0000'0000; // not given

    default:
      return 0x0000000000000000;
  }
}

}
