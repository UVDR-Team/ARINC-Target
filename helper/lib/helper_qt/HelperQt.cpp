// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace HelperQt.
 **/

#include "HelperQt.hpp"

#include <helper/VersionInformation.hpp>

#include <QtGlobal>

namespace HelperQt {

Helper::VersionsInformation::value_type qtVersion()
{
  return {
    "qt",
    {
      "Qt Library",
        qVersion(),
        "LGPL-3.0-only",
        "https://www.qt.io/"
    }
  };
}

void setLogLevel( const spdlog::level::level_enum level )
{
  spdlog::set_level( level );
}

}
