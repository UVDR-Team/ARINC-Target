// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::Packets::OpcodeDescription.
 **/

#include "OpcodeDescription.hpp"
#include <ostream>

#include <boost/exception/exception.hpp>
#include <arinc_support/InputError.hpp>

namespace Arinc615a::Find::Packets {

OpcodeDescription::OpcodeDescription() :
  Description{
    { "Information Request", Opcode::InformationRequest },
    { "Information Answer",  Opcode::InformationAnswer }
  }
{
}

std::ostream& operator<<( std::ostream &stream, const Opcode opcode )
{
  const auto name = OpcodeDescription::instance().name(opcode);
  return stream.write(name.data(), static_cast<std::streamsize>(name.size()));
}

}
