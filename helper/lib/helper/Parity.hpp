// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Parity Calculation Template.
 **/

#ifndef HELPER_PARITY_HPP
#define HELPER_PARITY_HPP

#include <helper/Helper.hpp>

#include <concepts>

namespace Helper {

/**
 * @brief Calculates the parity of the given value.
 *
 * @tparam ValueT
 *   Value type.
 * @tparam evenParity
 *   If the calculated value leads to an even or odd number of set bits.
 *
 * @param[in] value
 *   The value, for which parity shall be calculated.
 *
 * @return Calculated parity.
 **/
template< std::unsigned_integral ValueT, bool evenParity >
[[nodiscard]] constexpr bool calculateParity( ValueT value );

}

#include <helper/Parity.ipp>

#endif
