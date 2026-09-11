// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of BCD Conversion Functions.
 **/

#ifndef HELPER_BCD_HPP
#define HELPER_BCD_HPP

#include <helper/Helper.hpp>

#include <concepts>

namespace Helper {

/**
 * @name Binary-coded Decimal (BCD) Handling
 *
 * This Module provides template functions for converting unsigned integers to and from Binary Coded Decimals (BCD).
 *
 * This implementation provides support for Packed BCD encoding without sign handling.
 * This implementation expects all values to be unsigned.
 *
 * @sa https://en.wikipedia.org/wiki/Binary-coded_decimal
 *
 * @{
 **/

/**
 * @brief Converts @p value to BCD Representation.
 *
 * @tparam OutT
 *   Output type.
 *   Must be an unsigned integer type.
 * @tparam InT
 *   Input type.
 *   Must be an unsigned integer type.
 * @param[in] value
 *   Value to be converted to BCD representation.
 *
 * @return BCD representation of @p value.
 *
 * @sa @ref fromBcd
 **/
template< std::unsigned_integral OutT, std::unsigned_integral InT >
[[nodiscard]] constexpr OutT toBcd( InT value );

/**
 * @brief Decodes @p value from BCD Representation.
 *
 * @tparam OutT
 *   Output type.
 *   Must be an unsigned integer type.
 * @tparam InT
 *   Input type.
 *   Must be an unsigned integer type.
 * @param[in] value
 *   Value to be converted from BCD representation.
 *
 * @return Decoded @p value.
 *
 * @throw std::range_error
 *   When the digit does not lie between `0..9`.
 *
 * @sa @ref toBcd
 **/
template< std::unsigned_integral OutT, std::unsigned_integral InT >
[[nodiscard]] constexpr OutT fromBcd( InT value );

/** @} **/

}

#include <helper/Bcd.ipp>

#endif
