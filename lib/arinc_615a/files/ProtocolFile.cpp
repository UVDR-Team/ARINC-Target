// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::ProtocolFile.
 **/

#include "ProtocolFile.hpp"

#include <arinc_615a/Arinc615aException.hpp>

#include <arinc_support/Exception.hpp>

#include <boost/exception/all.hpp>

namespace Arinc615a::Files {

ProtocolFile::operator ArincSupport::RawData() const
{
  return encode();
}

Arinc615aVersion ProtocolFile::protocolVersion() const
{
  return protocolVersionV;
}

void ProtocolFile::protocolVersion( const Arinc615aVersion protocolVersion )
{
  protocolVersionV = protocolVersion;
}

ProtocolFile::ProtocolFile( const Arinc615aVersion protocolVersion ) :
  protocolVersionV{ protocolVersion }
{
}

void ProtocolFile::insertHeader( ArincSupport::RawDataSpan rawData ) const
{
  // file size
  const auto nextData{ ArincSupport::RawData_setInt( rawData, static_cast< uint32_t >( rawData.size() ) ) };

  // protocol version
  ArincSupport::RawData_setInt( nextData, std::to_underlying( protocolVersionV ) );
}

ArincSupport::ConstRawDataSpan ProtocolFile::decodeHeader( const ArincSupport::ConstRawDataSpan rawData )
{
  // check minimum data size
  if ( rawData.size() < HeaderSize )
  {
    BOOST_THROW_EXCEPTION(
      Arinc615aException() << ArincSupport::AdditionalInfo{ "Data packet to small" } );
  }

  // check length field
  uint32_t length{};
  auto remainingData{ rawData };
  std::tie( remainingData, length ) = ArincSupport::RawData_getInt< uint32_t >( remainingData );

  if ( length != rawData.size() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException()
      << ArincSupport::AdditionalInfo{ "internal length field and data size differs" } );
  }

  // protocol version
  uint16_t protocolVersion;
  std::tie( remainingData, protocolVersion ) = ArincSupport::RawData_getInt< uint16_t >( remainingData );

  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  switch ( Arinc615aVersion{ protocolVersion } )
  {
    case Arinc615aVersion::Arinc615a2:
    case Arinc615aVersion::Arinc615a34:
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc615aException()
        << ArincSupport::AdditionalInfo{ "Invalid or unsupported protocol version" } );
  }

  protocolVersionV = Arinc615aVersion{ protocolVersion };

  return remainingData;
}

}
