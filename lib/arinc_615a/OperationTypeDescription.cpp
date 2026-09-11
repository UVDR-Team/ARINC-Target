// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::OperationTypeDescription.
 **/

#include "OperationTypeDescription.hpp"

#include <boost/exception/exception.hpp>

#ifdef ARINC_615A_NO_PROGRAM_OPTIONS
#include <stdexcept>
#else
#include <boost/program_options.hpp>
#endif

namespace Arinc615a {

OperationTypeDescription::OperationTypeDescription():
  Description{
    { "Information",               OperationType::Information },
    { "Upload",                    OperationType::Upload },
    { "Media Defined Download",    OperationType::MediaDefinedDownload },
    { "Operator Defined Download", OperationType::OperatorDefinedDownload }
  }
{
}

std::ostream& operator<<( std::ostream &stream, const OperationType operation )
{
  return ( stream << OperationTypeDescription::instance().name( operation ) );
}

std::istream& operator>>( std::istream& stream, OperationType &operation )
{
  std::string operationStr;

  // extract string from stream
  stream >> operationStr;

  // Decode
  const auto optionalOperation{ OperationTypeDescription::instance().enumeration( operationStr ) };

  if ( !optionalOperation )
  {
#ifdef ARINC_615A_NO_PROGRAM_OPTIONS
    throw std::invalid_argument{ "Invalid Operation Type: " + operationStr };
#else
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value( operationStr ) );
#endif
  }

  operation = *optionalOperation;

  return stream;
}

}
