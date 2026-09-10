// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Files::MemoryFile.
 **/

#include "MemoryFile.hpp"

#include <arinc_support/Logging.hpp>

namespace Tftp::Files {

MemoryFile::MemoryFile() :
  operationV{ Operation::Receive },
  dataPtr{ dataV.begin() }
{
}

MemoryFile::MemoryFile( ArincSupport::ConstRawDataSpan data ) :
  operationV{ Operation::Transmit },
  dataV{ data.begin(), data.end() },
  dataPtr{ dataV.begin() }
{
}

MemoryFile::MemoryFile( ArincSupport::RawData data ) :
  operationV{ Operation::Transmit },
  dataV{ std::move( data ) },
  dataPtr{ dataV.begin() }
{
}

void MemoryFile::start()
{
  if ( Operation::Receive == operationV )
  {
    dataV.clear();
  }

  dataPtr = dataV.begin();
}

ArincSupport::ConstRawDataSpan MemoryFile::data() const noexcept
{
  return dataV;
}

void MemoryFile::finished() noexcept
{
  dataPtr = dataV.begin();
}

bool MemoryFile::receivedTransferSize( const uint64_t transferSize )
{
  ARINC_LOG_INFO( "Received transfer size: {}", transferSize );

  return true;
}

void MemoryFile::receivedData( ArincSupport::ConstRawDataSpan data )
{
  if ( !data.empty() )
  {
    dataV.insert( dataV.end(), data.begin(), data.end() );

    dataPtr = dataV.begin();
  }
}

std::optional< uint64_t> MemoryFile::requestedTransferSize()
{
  return dataV.size();
}

ArincSupport::RawData MemoryFile::sendData( const size_t maxSize )
{
  ArincSupport::RawData::const_iterator startPtr{ dataPtr };
  ArincSupport::RawData::const_iterator endPtr;

  if ( static_cast< size_t >( std::distance< ArincSupport::RawData::const_iterator >( startPtr, dataV.end() ) ) <= maxSize )
  {
    endPtr = dataV.end();
  }
  else
  {
    endPtr = dataPtr + static_cast< ptrdiff_t >( maxSize );
  }

  dataPtr = endPtr;

  return { startPtr, endPtr };
}

}
