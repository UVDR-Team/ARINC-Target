// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Subtype full type template
 *
 * @sa @ref Helper::generateBitMask()
 * @sa @ref Helper::subToFull()
 * @sa @ref Helper::fullToSub()
 **/

#ifndef HELPER_NBITINTEGER_HPP
#define HELPER_NBITINTEGER_HPP

#include <helper/Helper.hpp>

#include <concepts>

namespace Helper {

/**
 * @name Bit Handling
 * @{
 **/

/**
 * @brief Generates a bit mask.
 *
 * @tparam IntT
 *   Integer type which is used as base.
 *
 * @param[in] minBit
 *   First bit to set.
 * @param[in] maxBit
 *   Last bit to set.
 *
 * Ensure that minBit <= maxBit.
 *
 * Ensure that maxBit, and minBit is in range of IntT.
 *
 * @return Value where each bit between [minBit, maxBit] is set.
 *
 * @throw std::invalid_argument
 *   When `minBit > maxBit`
 * @throw std::invalid_argument
 *   @p maxBit is not in Range of @p IntT
 **/
template< std::unsigned_integral IntT >
[[nodiscard]] constexpr IntT generateBitMask( unsigned int minBit, unsigned int maxBit );

/**
 * @brief Converts a subtype to its full type.
 *
 * Subtype it means the value which is placed at any position within the full type.
 *
 * @tparam FullT
 *   Type, which contains the smaller subtype
 * @tparam SubT
 *   Type, which is placed in the greater full type.
 *
 * @param[in] sub
 *   Value of the subtype.
 * @param[in] minBit
 *   Starting bit of the subtype within the full type.
 * @param[in] maxBit
 *   Ending bit of the subtype within the full type.
 * @param[in] full
 *   Base full value which is or'ed with the subtype value.
 *
 * @return @p full | @p sub(moved to its configured position)
 **/
template< std::unsigned_integral FullT, std::unsigned_integral SubT >
requires ( std::numeric_limits< FullT >::digits >= std::numeric_limits< SubT >::digits )
[[nodiscard]] constexpr FullT subToFull( SubT sub, unsigned int minBit, unsigned int maxBit, FullT full = 0 );

/**
 * @brief Extracts the subtype from its full type.
 *
 * @tparam SubT
 *   Type, which is placed in the greater full type.
 * @tparam FullT
 *   Type, which contains the smaller subtype
 *
 * @param[in] full
 *   Full type value.
 * @param[in] minBit
 *   The starting bit of the subtype within the full type.
 * @param[in] maxBit
 *   The ending bit of the subtype within the full type.
 *
 * @return Contained subtype value.
 **/
template< std::unsigned_integral SubT, std::unsigned_integral FullT >
requires ( std::numeric_limits< FullT >::digits >= std::numeric_limits< SubT >::digits )
[[nodiscard]] constexpr SubT fullToSub( FullT full, unsigned int minBit, unsigned int maxBit );

/** @} **/

}

#include <helper/NBitInteger.ipp>

#endif
