// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Helper::EndianDescription.
 **/

#include "EndianDescription.hpp"

#include <boost/exception/exception.hpp>

#ifdef ARINC_615A_NO_PROGRAM_OPTIONS
#include <stdexcept>
#else
#include <boost/program_options.hpp>
#endif

namespace Helper {

std::endian EndianDescription::toEndian( const boost::endian::order order )
{
  switch ( order )
  {
    case boost::endian::order::big:
      return std::endian::big;

    case boost::endian::order::little:
      return std::endian::little;

    default:
      return std::endian::native;
  }
}

boost::endian::order EndianDescription::toEndian( const std::endian endian )
{
  switch ( endian )
  {
    case std::endian::big:
      return boost::endian::order::big;

    case std::endian::little:
      return boost::endian::order::little;

    default:
      return boost::endian::order::native;
  }
}

EndianDescription::EndianDescription() :
  Description{
    { "big",    std::endian::big },
    { "little", std::endian::little }
  }
{
}

std::ostream& operator<<( std::ostream &stream, const std::endian endian )
{
  return ( stream << EndianDescription::instance().name( endian ) );
}

std::istream& operator>>( std::istream &stream, std::endian &endian )
{
  std::string endianStr;
  stream >> endianStr;

  const auto optionalEndian{ EndianDescription::instance().enumeration( endianStr ) };

  if ( !optionalEndian )
  {
#ifdef ARINC_615A_NO_PROGRAM_OPTIONS
    throw std::invalid_argument{ "Invalid endian: " + endianStr };
#else
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value{ endianStr } );
#endif
  }

  endian = *optionalEndian;

  return stream;
}

}
