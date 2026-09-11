// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::MediumNumber.
 **/

#include "MediumNumber.hpp"

#include <ostream>

namespace Arinc665 {

MediumNumber::MediumNumber( const uint8_t mediumNumber ) noexcept :
  mediumNumberV{ ( mediumNumber == 0U ) ? static_cast< uint8_t >( 1U ) : mediumNumber }
{
}

std::string MediumNumber::toString() const
{
  return std::format( "{:03d}", mediumNumberV );
}

MediumNumber MediumNumber::operator+( uint8_t const increment ) const noexcept
{
  //! @todo use std::add_sat() if C++26 is available
  if ( mediumNumberV > ( 255U - increment ) )
  {
    MediumNumber{ 255U };
  }

  return MediumNumber( mediumNumberV + increment );
}

MediumNumber MediumNumber::operator-( uint8_t const decrement ) const noexcept
{
  if ( decrement >= mediumNumberV )
  {
    return MediumNumber{ 1U };
  }

  return MediumNumber( mediumNumberV - decrement );
}

MediumNumber& MediumNumber::operator++() noexcept
{
  if ( mediumNumberV < 255U )
  {
    ++mediumNumberV;
  }
  return *this;
}

MediumNumber MediumNumber::operator++( int ) noexcept
{
  const auto old{ *this };
  if ( mediumNumberV < 255U )
  {
    ++mediumNumberV;
  }
  return old;
}

MediumNumber& MediumNumber::operator--() noexcept
{
  if ( mediumNumberV > 1U )
  {
    --mediumNumberV;
  }
  return *this;
}

MediumNumber MediumNumber::operator--( int ) noexcept
{
  const auto old{ *this };
  if ( mediumNumberV > 1U )
  {
    --mediumNumberV;
  }
  return old;
}

MediumNumber::operator uint8_t() const noexcept
{
  return mediumNumberV;
}

MediumNumber::operator std::string() const
{
  return toString();
}

MediumNumber& MediumNumber::operator =( const uint8_t mediumNumber ) noexcept
{
  mediumNumberV = ( mediumNumber == 0U ) ? 1U : mediumNumber;
  return *this;
}

std::ostream &operator<<( std::ostream &stream, const MediumNumber &mediumNumber )
{
  return stream << mediumNumber.toString();
}

}
