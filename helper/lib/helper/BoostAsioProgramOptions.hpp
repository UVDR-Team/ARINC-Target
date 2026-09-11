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

#ifndef HELPER_BOOSTASIOPROGRAMOPTIONS_HPP
#define HELPER_BOOSTASIOPROGRAMOPTIONS_HPP

#include <helper/Helper.hpp>

#ifndef ARINC_615A_NO_PROGRAM_OPTIONS
#include <boost/asio/ip/address.hpp>
#include <iosfwd>

namespace boost::asio::ip {

HELPER_EXPORT std::istream& operator>>( std::istream &stream, boost::asio::ip::address &ipAddress );

}
#endif

#endif
