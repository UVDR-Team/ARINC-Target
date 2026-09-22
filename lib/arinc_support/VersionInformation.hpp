// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Struct ArincSupport::VersionInformation.
 **/

#ifndef ARINC_SUPPORT_VERSIONINFORMATION_HPP
#define ARINC_SUPPORT_VERSIONINFORMATION_HPP

#include <arinc_support/Support.hpp>

#include <string>

namespace ArincSupport {

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
