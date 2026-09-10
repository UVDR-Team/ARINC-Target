// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Helper for parsing boost::asio structs with boost::program_options
 **/

#include "BoostAsioProgramOptions.hpp"

#include <boost/program_options.hpp>

namespace boost::asio::ip {

std::istream& operator>>( std::istream &stream, boost::asio::ip::address &ipAddress )
{
  // extract string from stream
  std::string ipStr;
  stream >> ipStr;

  // convert IP address
  boost::system::error_code errorCode;
  ipAddress = boost::asio::ip::make_address( ipStr, errorCode );

  if ( errorCode )
  {
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value{ ipStr } );
  }

  return stream;
}

}
