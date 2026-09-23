// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::DataPacket.
 **/

#include "DataPacket.hpp"

#include <tftp/packets/PacketException.hpp>

#include <arinc_support/Exception.hpp>

#include <boost/exception/all.hpp>

#include <arinc_support/Format.hpp>
#include <utility>

namespace Tftp::Packets {

DataPacket::DataPacket( const BlockNumber blockNumber, Data data ) noexcept :
  Packet{ PacketType::Data },
  blockNumberV{ blockNumber },
  dataV{ std::move( data ) }
{
}

DataPacket::DataPacket( ArincSupport::ConstRawDataSpan rawPacket ) :
  Packet{ PacketType::Data, rawPacket }
{
  decodeBody( rawPacket );
}

DataPacket& DataPacket::operator=( ArincSupport::ConstRawDataSpan rawPacket )
{
  decodeHeader( rawPacket );
  decodeBody( rawPacket );
  return *this;
}

BlockNumber DataPacket::blockNumber() const
{
  return blockNumberV;
}

BlockNumber& DataPacket::blockNumber()
{
  return blockNumberV;
}

void DataPacket::blockNumber( const BlockNumber blockNumber )
{
  blockNumberV = blockNumber;
}

const DataPacket::Data& DataPacket::data() const
{
  return dataV;
}

DataPacket::Data& DataPacket::data()
{
  return dataV;
}

void DataPacket::data( Data data )
{
  dataV = std::move( data );
}

size_t DataPacket::dataSize() const
{
  return dataV.size();
}

DataPacket::operator std::string() const
{
  return ArincSupport::format( "DATA: Block No: {} DATA: {} bytes", static_cast< uint16_t >( blockNumber() ), dataSize() );
}

ArincSupport::RawData DataPacket::encode() const
{
  ArincSupport::RawData rawPacket( MinPacketSize + dataV.size() );

  insertHeader( rawPacket );

  auto rawSpan{ ArincSupport::RawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // block number
  rawSpan = ArincSupport::RawData_setInt( rawSpan, static_cast< uint16_t >( blockNumberV ) );
  assert( rawSpan.size() == dataV.size() );

  // data
  std::copy( dataV.begin(), dataV.end(), rawSpan.begin() );

  return rawPacket;
}

void DataPacket::decodeBody( ArincSupport::ConstRawDataSpan rawPacket )
{
  // check size
  if ( rawPacket.size() < MinPacketSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << ArincSupport::AdditionalInfo{ "Invalid packet size of DATA packet" } );
  }

  auto remaining{ ArincSupport::ConstRawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // decode block number
  std::tie( remaining, static_cast< uint16_t & >( blockNumberV ) ) = ArincSupport::RawData_getInt< uint16_t >( remaining );

  // copy data
  dataV.assign( remaining.begin(), remaining.end() );
}

}
