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

#ifndef ARINC_SUPPORT_BOOST_ASIO_PROGRAM_OPTIONS_HPP
#define ARINC_SUPPORT_BOOST_ASIO_PROGRAM_OPTIONS_HPP

#include <arinc_support/Support.hpp>

#include <boost/asio/ip/address.hpp>

#include <iosfwd>

namespace boost::asio::ip {

/**
 * @brief Parse an IP Address String to an ASIO IP Address.
 *
 * @param[in] stream
 *   Input Stream
 * @param[out] ipAddress
 *   Parsed IP address
 *
 * @return @p st
 *
 * @throw boost::program_options::invalid_option_value
 *   when given IP address cannot be parsed
 **/
ARINC_SUPPORT_EXPORT std::istream& operator>>( std::istream &stream, boost::asio::ip::address &ipAddress );

}

#endif
