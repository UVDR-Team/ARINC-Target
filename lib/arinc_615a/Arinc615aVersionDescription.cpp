// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Arinc615aVersionDescription.
 **/

#include "Arinc615aVersionDescription.hpp"
#include <ostream>

#include <boost/exception/exception.hpp>

#include <arinc_support/InputError.hpp>

namespace Arinc615a {

Arinc615aVersionDescription::Arinc615aVersionDescription() :
  Description{
    { "Arinc615a1",  Arinc615aVersion::Arinc615a1 },
    { "Arinc615a2",  Arinc615aVersion::Arinc615a2 },
    { "Arinc615a34", Arinc615aVersion::Arinc615a34 }
  }
{
}

std::ostream& operator<<( std::ostream &stream, Arinc615aVersion version )
{
  const auto name = Arinc615aVersionDescription::instance().name(version);
  return stream.write(name.data(), static_cast<std::streamsize>(name.size()));
}

std::istream& operator>>( std::istream& stream, Arinc615aVersion &version )
{
  std::string versionStr;

  // extract string from stream
  stream >> versionStr;

  // Decode
  const auto optionalVersion{ Arinc615aVersionDescription::instance().enumeration( versionStr ) };

  if ( !optionalVersion )
  {
    BOOST_THROW_EXCEPTION( ArincSupport::InvalidOptionValue{ versionStr } );
  }

  version = *optionalVersion;

  return stream;
}

}
