// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Template Function CheckCrc.
 **/

#ifndef ARINC_649_TEST_ARINC649CRCTEST_HPP
#define ARINC_649_TEST_ARINC649CRCTEST_HPP

#include <arinc_649/Arinc649Crc.hpp>

#include <helper/RawData.hpp>

#include <boost/test/unit_test.hpp>

#include <vector>

/**
 * @brief Check CRC Template
 *
 * @tparam CrcT
 *   CRC Type
 *
 * @param[in] data
 *   Raw Data
 * @param[in] expectedCrc
 *   Expected CRC
 **/
template< typename CrcT >
static void CheckCrc( Helper::ConstRawDataSpan data, typename CrcT::value_type expectedCrc )
{
  CrcT crc;

  crc.reset();

  crc.process_bytes( std::data( data ), std::size( data ) );
  BOOST_CHECK_EQUAL( crc.checksum(), expectedCrc );
}

#endif
