// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Memory Dump Interface.
 **/

#include "Dump.hpp"

#include <cctype>
#include <cstdint>
#include <helper/Format.hpp>

namespace Helper {

std::string Dump( void const * const address, const std::size_t length, const std::size_t lineWidth )
{
  auto const * const CharAddress{ static_cast< uint8_t const * >( address ) };

  std::string result;

  std::string hexView;
  std::string asciiView;

  std::locale locale{};

  for ( unsigned int index = 0; index < length; ++index )
  {
    hexView += ARINC_FORMAT_NAMESPACE::format( "{:02X} ", static_cast< unsigned int >( CharAddress[ index ] ) );

    if ( 0 != std::isprint( CharAddress[ index ] ) )
    {
      asciiView += static_cast< char>( CharAddress[ index ] );
    }
    else
    {
      asciiView += '.';
    }

    // If End Of Memory, then fill strings to right size
    if ( index + 1 == length )
    {
      hexView.resize( 3 * lineWidth, ' ' );
      asciiView.resize( lineWidth, ' ' );
    }

    // If Last Character of Line or of Memory reached
    if ( ( lineWidth - 1 == index % lineWidth ) || ( index + 1 == length ) )
    {
      result += ARINC_FORMAT_NAMESPACE::format(
        "{:08X}  {} |{}|\n",
        ( lineWidth * ( index / lineWidth ) ),
        hexView,
        asciiView );
      hexView.clear();
      asciiView.clear();
    }
  }

  return result;
}

}
