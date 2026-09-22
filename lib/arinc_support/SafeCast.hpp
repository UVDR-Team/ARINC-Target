// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Safe Cast Template.
 *
 * @sa safeCast()
 **/

#ifndef ARINC_SUPPORT_SAFECAST_HPP
#define ARINC_SUPPORT_SAFECAST_HPP

#include <arinc_support/Support.hpp>

#include <concepts>
#include <limits>

namespace ArincSupport {

/**
 * @name Casting
 * @{
 **/

/**
 * @brief Performs safe cast of @p source from @p SourceT to @p TargetT.
 *
 * Checks if the parameter @p source fits in the range of the type @p TargetT and performs cast.
 *
 * @tparam SourceT
 *   Type of Source Value.
 * @tparam TargetT
 *   Type of Target Value.
 *
 * @param[in] source
 *   Source value to be cast.
 *
 * @return @p source cast to @p TargetT
 *
 * @throws std::range_error
 *   When the parameter @p source does not fit into type @p TargetT.
 **/
template< std::integral TargetT, std::integral SourceT >
requires std::is_convertible_v< SourceT, TargetT >
[[nodiscard]] constexpr TargetT safeCast( SourceT source );

/**
 * @brief Checks that Parameter @p source is of Type @p TargetT and in Range @p [min,max].
 *
 * @tparam TargetT
 *   Target type
 * @tparam min
 *   Minimum value
 * @tparam max
 *   Maximum value
 * @tparam SourceT
 *   Source type
 *
 * @param[in] source
 *   Value to check.
 *
 * @return if the parameter @p source is of type @p TargetT and in range @p [min,max]
 **/
template<
  std::integral TargetT,
  TargetT min = std::numeric_limits< TargetT >::min(),
  TargetT max = std::numeric_limits< TargetT >::max(),
  std::integral SourceT >
[[nodiscard]] constexpr bool inRange( SourceT source ) noexcept;

/** @} **/

}

#include <arinc_support/SafeCast.ipp>

#endif
