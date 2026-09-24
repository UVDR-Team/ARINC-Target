// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ Definition of Class ArincSupport::Exception.
 **/

#ifndef ARINC_SUPPORT_EXCEPTION_HPP
#define ARINC_SUPPORT_EXCEPTION_HPP

#include <arinc_support/Support.hpp>

#include <boost/exception/error_info.hpp>

#include <string>

namespace ArincSupport {

//! Additional Info Type, which adds a String Additional Information.
using AdditionalInfo = boost::error_info< struct AdditionalInfoTag, std::string >;

}

#endif
