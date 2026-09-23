// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc665.
 **/

#include "Arinc665.hpp"

#include <arinc_665/Version.hpp>

#include <arinc_support/VersionInformation.hpp>

namespace Arinc665 {

ArincSupport::VersionsInformation::value_type arinc665Version()
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

void setLogLevel( const ArincSupport::Logging::level::level_enum level )
{
  ArincSupport::Logging::set_level( level );
}

}
