// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc649.
 **/

#include "Arinc649.hpp"

#include <arinc_649/Version.hpp>

#include <helper/VersionInformation.hpp>

namespace Arinc649 {

Helper::VersionsInformation::value_type arinc649Version()
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
