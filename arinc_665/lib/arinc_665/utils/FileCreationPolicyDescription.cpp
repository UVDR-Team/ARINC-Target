// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::FileCreationPolicyDescription.
 **/

#include <arinc_665/utils/FileCreationPolicyDescription.hpp>

#include <boost/exception/exception.hpp>

#include <boost/program_options.hpp>

namespace Arinc665::Utils {

FileCreationPolicyDescription::FileCreationPolicyDescription():
  Description{
    { "None",         FileCreationPolicy::None },
    { "NoneExisting", FileCreationPolicy::NoneExisting },
    { "All",          FileCreationPolicy::All }
  }
{
}

std::string FileCreationPolicyDescription::allValues( std::string_view prefix ) const
{
  std::string allValues{ prefix };
  for ( const auto &description : descriptions().get< ByEnum >() )
  {
    allValues += std::format( "\n\t- {}", description.name );
  }
  return allValues;
}

std::ostream& operator<<( std::ostream &stream, const FileCreationPolicy fileCreationPolicy )
{
  return ( stream << FileCreationPolicyDescription::instance().name( fileCreationPolicy ) );
}

std::istream& operator>>( std::istream &stream, FileCreationPolicy &fileCreationPolicy )
{
  std::string str;

  // extract string from the stream
  stream >> str;

  // Decode
  const auto optionalFileCreationPolicy{ FileCreationPolicyDescription::instance().enumeration( str ) };

  if ( !optionalFileCreationPolicy )
  {
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value( str ) );
  }

  fileCreationPolicy = *optionalFileCreationPolicy;
  return stream;
}

}
