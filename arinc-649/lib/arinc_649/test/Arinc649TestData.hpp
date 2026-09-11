// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Arinc649TestData.
 **/

#ifndef ARINC_649_TEST_ARINC649TESTDATA_HPP
#define ARINC_649_TEST_ARINC649TESTDATA_HPP

#include <arinc_649/Arinc649.hpp>

#include <span>
#include <cstddef>

namespace Arinc649 {

//! ARINC 649 Test Data
enum class TestData
{
  //! Empty Data
  t01a,
  //! 128 Bytes of 0xFF
  t02a,
  //! 100 Bytes of 0x00
  t03a,
  //! 128 16bit-words of 0xAA55
  t04a,
  //! 256 Bytes of 0x00 through 0xFF
  t11a,
  //! String "ACM1234578"
  t12a,
  //! String "ABCDEFGHIJKLMNO"
  t13a,
};

/**
 * @brief Returns Test Data According to ARINC 649 Specification.
 *
 * @param[in] testData
 *   Requested Test Data.
 *
 * @return Test Data Span
 **/
std::span< const std::byte > getTestData( TestData testData );

/**
 * @brief Returns Expected CRC Values for CRC-8.
 *
 * @param[in] testData
 *   Requested Test Data.
 *
 * @return Expected CRC-8 Value
 **/
uint8_t getTestDataCrc8( TestData testData );

/**
 * @brief Returns Expected CRC Values for CRC-16.
 *
 * @param[in] testData
 *   Requested Test Data.
 *
 * @return Expected CRC-16 Value
 **/
uint16_t getTestDataCrc16( TestData testData );

/**
 * @brief Returns Expected CRC Values for CRC-32.
 *
 * @param[in] testData
 *   Requested Test Data.
 *
 * @return Expected CRC-32 Value
 **/
uint32_t getTestDataCrc32( TestData testData );

/**
 * @brief Returns Expected CRC Values for CRC-64.
 *
 * @param[in] testData
 *   Requested Test Data.
 *
 * @return Expected CRC-64 Value
 **/
uint64_t getTestDataCrc64( TestData testData );

}

#endif
