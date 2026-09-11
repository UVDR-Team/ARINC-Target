// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Helper.
 **/

#include "Helper.hpp"

#include <helper/Version.hpp>
#include <helper/VersionInformation.hpp>

#include <boost/version.hpp>

namespace Helper {

VersionsInformation::value_type boostVersion()
{
  return {
    "boost",
    {
      "Boost Library",
        BOOST_LIB_VERSION,
        "Boost Software License",
        "https://www.boost.org/"
    }
  };
}

VersionsInformation::value_type helperVersion()
{
  return {
    Version::Key,
    {
      Version::Name,
      Version::VersionInformation,
      Version::License,
      Version::Url
    }
  };
}

void setLogLevel( const spdlog::level::level_enum level )
{
  spdlog::set_level( level );
}

}
