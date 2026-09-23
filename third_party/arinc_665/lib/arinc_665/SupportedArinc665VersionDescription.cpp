// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::SupportedArinc665VersionDescription.
 **/

#include "SupportedArinc665VersionDescription.hpp"
#include <ostream>

#include <boost/exception/exception.hpp>
#include <arinc_support/InputError.hpp>

#include <istream>
#include <ostream>

namespace Arinc665 {

SupportedArinc665VersionDescription::SupportedArinc665VersionDescription() :
  Description{
    { "Supplement2",   SupportedArinc665Version::Supplement2 },
    { "Supplement345", SupportedArinc665Version::Supplement345 }
  }
{
}

std::string SupportedArinc665VersionDescription::allValues( std::string_view prefix ) const
{
  static const std::string versionValues{
  "- '" + std::string{ name( SupportedArinc665Version::Supplement2 ) } +   "': ARINC 665-2\n" +
  "- '" + std::string{ name( SupportedArinc665Version::Supplement345 ) } +  "': ARINC 665-3/4/5" };

  return std::format( "{}\n{}", prefix, versionValues );
}

std::ostream& operator<<( std::ostream &stream, const SupportedArinc665Version version )
{
  const auto name = SupportedArinc665VersionDescription::instance().name(version);
  return stream.write(name.data(), static_cast<std::streamsize>(name.size()));
}

std::istream& operator>>( std::istream &stream, SupportedArinc665Version &version )
{
  std::string versionStr;

  // extract string from stream
  stream >> versionStr;

  // Decode
  const auto optionalVersion{ SupportedArinc665VersionDescription::instance().enumeration( versionStr ) };

  if ( !optionalVersion )
  {
    BOOST_THROW_EXCEPTION( ArincSupport::InvalidOptionValue( versionStr ) );
  }

  version = *optionalVersion;

  return stream;
}

}
