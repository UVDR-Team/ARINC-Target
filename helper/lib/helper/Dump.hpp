// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Memory Dump Interface.
 **/

#ifndef HELPER_DUMP_HPP
#define HELPER_DUMP_HPP

#include <helper/Helper.hpp>

#include <cstddef>
#include <string>

namespace Helper {

/**
 * @brief Dumps the given memory to a printable string.
 *
 * @param[in] address
 *   Start of memory to dump
 * @param[in] length
 *   Length of Memory to dump
 * @param[in] lineWidth
 *   Line width of dump output
 *
 * @return Formatted memory dump as string
 **/
[[nodiscard]] HELPER_EXPORT std::string Dump( void const * address, std::size_t length, std::size_t lineWidth = 16U );

}

#endif
