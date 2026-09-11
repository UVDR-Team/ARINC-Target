// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Struct Helper::VersionInformation.
 **/

#ifndef HELPER_VERSIONINFORMATION_HPP
#define HELPER_VERSIONINFORMATION_HPP

#include <helper/Helper.hpp>

#include <string>

namespace Helper {

//! Version Information
struct VersionInformation
{
  //! Product Name
  std::string productName;
  //! Product Version
  std::string productVersion;
  //! Product License
  std::string productLicense;
  //! Product URL
  std::string productUrl;
};

}

#endif
