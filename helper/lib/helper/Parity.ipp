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

#include <bit>

namespace Helper {

template< std::unsigned_integral ValueT, bool evenParity >
constexpr bool calculateParity( const ValueT value )
{
  const auto nBits{ std::popcount( value ) };

  /* Even Parity | NBits is Even || Parity
   * TRUE        | TRUE          || FALSE
   * TRUE        | FALSE         || TRUE
   * FALSE       | TRUE          || TRUE
   * FALSE       | FALSE         || FALSE
   */
  return ( evenParity != ( nBits % 2U == 0U ) );
}

}
