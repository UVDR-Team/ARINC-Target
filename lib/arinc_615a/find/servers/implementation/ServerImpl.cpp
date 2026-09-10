// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Find::Servers::ServerImpl.
 **/

#include "ServerImpl.hpp"

#include <arinc_615a/find/packets/Packet.hpp>
#include <arinc_615a/find/packets/PacketStatistic.hpp>

#include <arinc_615a/find/servers/FindServerException.hpp>

#include <arinc_615a/find/TargetInformation.hpp>
#include <arinc_615a/find/FindConfiguration.hpp>

#include <arinc_support/Exception.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/exception/all.hpp>

#include <format>

namespace Arinc615a::Find::Servers {

ServerImpl::ServerImpl( boost::asio::io_context &ioContext ) :
  socketV{ ioContext },
  rawReceivePacketV( Packets::Packet::MaxPacketSize )
{
}

ServerImpl::~ServerImpl() noexcept
{
  try
  {
    if ( socketV.is_open() )
    {
      socketV.close();
    }
  }
  catch ( const boost::system::system_error &err )
  {
    ARINC_LOG_ERROR( "Error stopping FIND server: {}", err.what() );

    // do nothing - ignore error.
  }
}

Server& ServerImpl::requestHandler( FindRequestHandler requestHandler )
{
  requestHandlerV = std::move( requestHandler );
  return *this;
}

Server& ServerImpl::localEndpoint( boost::asio::ip::udp::endpoint localEndpoint )
{
  localEndpointV = std::move( localEndpoint );
  return *this;
}

void ServerImpl::start()
{
  ARINC_LOG_INFO( "Start FIND Server on {}:{}", localEndpointV.address().to_string(), localEndpointV.port() );

  try
  {
    socketV.open( localEndpointV.protocol() );
    socketV.bind( localEndpointV );

    // start receive
    receive();
  }
  catch ( const boost::system::system_error &err )
  {
    // close socket when opened
    if ( socketV.is_open() )
    {
      socketV.close();
    }

    BOOST_THROW_EXCEPTION( FindServerException() << ArincSupport::AdditionalInfo{ err.what() } );
  }
}

void ServerImpl::stop()
{
  ARINC_LOG_INFO( "Stop FIND Server" );

  // cancel receive operation and close socket
  socketV.cancel();
  socketV.close();
}

void ServerImpl::response( const boost::asio::ip::udp::endpoint &remote, const TargetInformation &findInformation )
{
  ARINC_LOG_INFO( "Send FIND Response to {}:{}", remote.address().to_string(), remote.port() );

  // generate response packet
  Packets::Packet answerPacket{
    Packets::Opcode::InformationAnswer,
    Packets::Packet::Parameters{
      findInformation.thwId,
      findInformation.thwTypeName,
      findInformation.thwPosition,
      findInformation.literalName,
      findInformation.manufacturerCode } };

  auto rawPacket{ answerPacket.encode() };

  // Update statistic
  Packets::PacketStatistic::globalTransmit().packet( answerPacket.opcode(), rawPacket.size() );

  // send response
  socketV.send_to( boost::asio::buffer( rawPacket ), remote );
}

void ServerImpl::receive()
{
  // resize packet
  rawReceivePacketV.resize( Packets::Packet::MaxPacketSize );

  // start receive operation
  socketV.async_receive_from(
    boost::asio::buffer( rawReceivePacketV ),
    receiveRemoteEndpointV,
    std::bind_front( &ServerImpl::receiveHandler, this ) );
}

void ServerImpl::receiveHandler( const boost::system::error_code &error, const std::size_t bytesTransferred )
{
  if ( boost::asio::error::operation_aborted == error )
  {
    // read operation has been aborted - do nothing
    return;
  }

  if ( !error )
  {
    packet( receiveRemoteEndpointV, { rawReceivePacketV.data(), bytesTransferred } );
  }
  else
  {
    ARINC_LOG_ERROR( "Error when receiving message: {}", error.message() );
  }

  // restart receive operation
  receive();
}

void ServerImpl::informationRequestPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::Packet &request )
{
  ARINC_LOG_INFO( "FIND Request from {}:{}", remote.address().to_string(), remote.port() );

  if ( ( request.numberOfParameters() != 1 ) && ( !request.parameter( 0 ).empty() ) )
  {
    ARINC_LOG_INFO( "Invalid FIND Request" );
    return;
  }

  // call receiveFindRequest handler and obtain FIND information
  if ( requestHandlerV )
  {
    requestHandlerV( remote );
  }
}

void ServerImpl::informationAnswerPacket(
  const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] const Packets::Packet &answer )
{
  ARINC_LOG_WARN(
      "FIND information answer packet from {}:{} received - IGNORE it",
      remote.address().to_string(),
      remote.port() );
}

void ServerImpl::invalidPacket(
  const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] ArincSupport::ConstRawDataSpan rawPacket )
{
  ARINC_LOG_WARN(
      "invalid packet received from {}:{} - IGNORE it",
      remote.address().to_string(),
      remote.port() );
}

}
