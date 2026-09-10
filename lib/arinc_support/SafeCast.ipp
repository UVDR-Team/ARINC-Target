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
 **/

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace ArincSupport {

template< std::integral TargetT, std::integral SourceT >
requires std::is_convertible_v< SourceT, TargetT >
[[nodiscard]] constexpr TargetT safeCast( const SourceT source )
{
  if ( !std::in_range< TargetT >( source ) )
  {
    throw std::range_error{ "source value not in range of target type" };
  }

  return static_cast< TargetT >( source );
}

template<
  std::integral TargetT,
  TargetT min,
  TargetT max,
  std::integral SourceT >
constexpr bool inRange( const SourceT source ) noexcept
{
  return std::in_range< TargetT >( source )
    && ( static_cast< TargetT >( source ) >= min )
    && ( static_cast< TargetT >( source ) <= max );
}

}
