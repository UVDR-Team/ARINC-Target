// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Find::Packets::OpcodeDescription.
 **/

#ifndef ARINC_615A_FIND_PACKETS_OPCODEDESCRIPTION_HPP
#define ARINC_615A_FIND_PACKETS_OPCODEDESCRIPTION_HPP

#include <arinc_615a/find/packets/Packets.hpp>

#include <arinc_support/Description.hpp>

#include <arinc_support/Format.hpp>
#include <iosfwd>

namespace Arinc615a::Find::Packets {

/**
 * @name ARINC FIND Packet Opcode
 * @{
 **/

/**
 * @brief ARINC FIND Packet Opcode Description.
 *
 * @sa @ref Opcode
 **/
class ARINC_615A_EXPORT OpcodeDescription final : public ArincSupport::Description< OpcodeDescription, Opcode >
{
  public:
    //! Initialises Instance
    OpcodeDescription();
};

/**
 * @brief Streaming Operator to Output ARINC 615A Version as String
 *
 * @param[in,out] stream
 *   Output stream
 * @param[in] opcode
 *   ARINC 615A FIND Opcode
 *
 * @return @p stream for chaining.
 *
 * @sa @ref OpcodeDescription
 * @sa @ref Opcode
 **/
ARINC_615A_EXPORT std::ostream &operator<<( std::ostream &stream, Opcode opcode );

/** @} **/

}

#endif
