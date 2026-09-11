// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of String Utility Functions.
 **/

#ifndef ARINC_665_FILES_STRINGUTILS_HPP
#define ARINC_665_FILES_STRINGUTILS_HPP

#include <arinc_665/files/Files.hpp>

#include <helper/RawData.hpp>

#include <list>
#include <set>
#include <string>
#include <string_view>
#include <tuple>

namespace Arinc665::Files {

/**
 * @name ARINC 665 String Handling
 *
 * @par String Encoding
 *
 * Offset | Field   | Length   | Description
 * -------|---------|----------|------------------------------------------------
 * +0     | Length  | 2 Bytes  | Length of String
 * +2     | Value   | Variable | String (`Length` Bytes)
 * +x     | Padding | 0..1     | If string length is odd, add a `0` fill char
 *
 * @{
 **/

/**
 * @brief Decodes the ARINC 665 string from the raw data.
 *
 * Decodes length field and the following string.
 * Checks if the string is filled to a multiple of 2 bytes.
 *
 * @param[in] rawData
 *   Raw Data.
 *
 * @return std::tuple of Remaining Raw Data and decoded string.
 **/
ARINC_665_EXPORT std::tuple< Helper::ConstRawDataSpan, std::string_view > StringUtils_decodeString(
  Helper::ConstRawDataSpan rawData );

/**
 * @brief Encodes the ARINC 665 string to the stream.
 *
 * @param[in] string
 *  String to encode.
 *
 * @return Encoded raw string.
 **/
[[nodiscard]] ARINC_665_EXPORT Helper::RawData StringUtils_encodeString( std::string_view string );

/** @} **/

/**
 * @name ARINC 665 String List Handling.
 *
 * Offset | Field          | Length   | Description
 * -------|----------------|----------|-----------------------------------------
 * +0     | No. of Strings | 2 Bytes  | Number of Strings following this filed
 * +x     | String 1       | Variable | Encoded string
 * +y     | String x       | Variable | Encoded string
 *
 * @{
 **/

/**
 * @brief Decodes the ARINC 665 String List from the stream.
 *
 * @param[in] rawData
 *   raw string list.
 *
 * @return std::tuple of remaining raw data and decoded strings.
 **/
ARINC_665_EXPORT std::tuple< Helper::ConstRawDataSpan, std::list< std::string > > StringUtils_decodeStrings(
  Helper::ConstRawDataSpan rawData );

/**
 * @brief Encodes the ARINC 665 String List to the Stream.
 *
 * @param[in] strings
 *   String List
 *
 * @return Encoded raw string list.
 **/
[[nodiscard]] ARINC_665_EXPORT Helper::RawData StringUtils_encodeStrings( const std::list< std::string > &strings );

/** @} **/

}

#endif
