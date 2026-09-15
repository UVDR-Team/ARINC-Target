// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Tftp::Packets TftpOptions.
 **/

#include "TftpOptions.hpp"

#include <helper/Format.hpp>

namespace Tftp::Packets {

std::string_view TftpOptions_name( const KnownOptions option ) noexcept
{
  switch ( option )
  {
    case KnownOptions::BlockSize:
      return "blksize";

    case KnownOptions::Timeout:
      return "timeout";

    case KnownOptions::TransferSize:
      return "tsize";

    default:
      return {};
  }
}

std::string TftpOptions_toString( const TftpOptions &options )
{
  if ( !options )
  {
    return "(NONE)";
  }

  std::string retStr{};

  if ( options.blockSize )
  {
    retStr+= ARINC_FORMAT_NAMESPACE::format(
      "[{}:{}]",
      TftpOptions_name( KnownOptions::BlockSize ),
      std::to_string( *options.blockSize ) );
  }

  if ( options.timeout )
  {
    retStr+= ARINC_FORMAT_NAMESPACE::format(
      "[{}:{}]",
      TftpOptions_name( KnownOptions::Timeout ),
      *options.timeout );
  }

  if ( options.transferSize )
  {
    retStr+= ARINC_FORMAT_NAMESPACE::format(
      "[{}:{}]",
      TftpOptions_name( KnownOptions::TransferSize ),
      *options.transferSize );
  }

  return retStr;
}

std::ostream &operator<<( std::ostream &stream, const TftpOptions &options )
{
  return ( stream << TftpOptions_toString( options ) );
}

}
