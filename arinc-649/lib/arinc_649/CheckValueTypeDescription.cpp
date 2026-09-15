// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc649::CheckValueTypeDescription.
 **/

#include "CheckValueTypeDescription.hpp"

#include <boost/exception/exception.hpp>

#ifndef ARINC_615A_NO_PROGRAM_OPTIONS
#include <boost/program_options.hpp>
#else
#include <stdexcept>
#include <istream>
#endif

namespace Arinc649 {

CheckValueTypeDescription::CheckValueTypeDescription() :
  Description{
    { "NotUsed", CheckValueType::NotUsed },
    { "CRC8",    CheckValueType::Crc8 },
    { "CRC16",   CheckValueType::Crc16 },
    { "CRC32",   CheckValueType::Crc32 },
    { "MD5",     CheckValueType::Md5 },
    { "SHA1",    CheckValueType::Sha1 },
    { "SHA256",  CheckValueType::Sha256 },
    { "SHA512",  CheckValueType::Sha512 },
    { "CRC64",   CheckValueType::Crc64 }
  }
{
}

std::ostream& operator<<( std::ostream &stream, const CheckValueType type )
{
  return stream << CheckValueTypeDescription::instance().name( type );
}

std::istream& operator>>( std::istream& stream, CheckValueType &type )
{
  std::string checkValueTypeStr{};

  // extract string from the stream
  stream >> checkValueTypeStr;

  // Decode
  const auto optionalType{ CheckValueTypeDescription::instance().enumeration( checkValueTypeStr ) };

  if ( !optionalType )
  {
#ifndef ARINC_615A_NO_PROGRAM_OPTIONS
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value( checkValueTypeStr ) );
#else
    throw std::invalid_argument("Invalid check value type: " + checkValueTypeStr);
#endif
  }

  type = *optionalType;

  return stream;
}

}
