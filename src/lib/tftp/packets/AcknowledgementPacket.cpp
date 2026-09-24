// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::AcknowledgementPacket.
 **/

#include "AcknowledgementPacket.hpp"

#include <tftp/packets/PacketException.hpp>

#include <arinc_support/Exception.hpp>
#include <arinc_support/RawData.hpp>

#include <boost/exception/all.hpp>

#include <arinc_support/Format.hpp>
#include <utility>

namespace Tftp::Packets {

AcknowledgementPacket::AcknowledgementPacket( const BlockNumber blockNumber ) noexcept :
  Packet{ PacketType::Acknowledgement },
  blockNumberV{ blockNumber }
{
}

AcknowledgementPacket::AcknowledgementPacket( ArincSupport::ConstRawDataSpan rawPacket ) :
  Packet{ PacketType::Acknowledgement, rawPacket }
{
  decodeBody( rawPacket );
}

AcknowledgementPacket& AcknowledgementPacket::operator=( ArincSupport::ConstRawDataSpan rawPacket )
{
  decodeHeader( rawPacket );
  decodeBody( rawPacket );
  return *this;
}

BlockNumber AcknowledgementPacket::blockNumber() const
{
  return blockNumberV;
}

void AcknowledgementPacket::blockNumber( const BlockNumber blockNumber )
{
  blockNumberV = blockNumber;
}

AcknowledgementPacket::operator std::string() const
{
  return ArincSupport::format( "ACK: BLOCK NO: {}", static_cast< uint16_t>( blockNumberV ) );
}

ArincSupport::RawData AcknowledgementPacket::encode() const
{
  ArincSupport::RawData rawPacket( PacketSize );

  // insert header data
  insertHeader( rawPacket );

  auto rawSpan{ ArincSupport::RawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // block number
  rawSpan = ArincSupport::RawData_setInt( rawSpan, static_cast< uint16_t >( blockNumberV ) );
  assert( rawSpan.empty() );

  return rawPacket;
}

void AcknowledgementPacket::decodeBody( ArincSupport::ConstRawDataSpan rawPacket )
{
  // check size
  if ( rawPacket.size() != PacketSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << ArincSupport::AdditionalInfo{ "Invalid packet size of ACK packet" } );
  }

  auto rawSpan{ ArincSupport::ConstRawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // decode block number
  std::tie( rawSpan, static_cast< uint16_t & >( blockNumberV ) ) = ArincSupport::RawData_getInt< uint16_t >( rawSpan );
  assert( rawSpan.empty() ); // Keep assertion, as otherwise above runtime check would be wrong
}

}
