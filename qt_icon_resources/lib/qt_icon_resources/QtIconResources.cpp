// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace QtIconResources.
 **/

#include "QtIconResources.hpp"

#include <qt_icon_resources/Version.hpp>

#include <helper/VersionInformation.hpp>

#include <QDir>

/**
 * @brief Initialises the resources.
 **/
static void Resources_initialise();

namespace QtIconResources {

void initialise()
{
  Resources_initialise();
}

/**
 * @brief Provide Version Qt Icon Resources Version Information.
 *
 * @return Version Information.
 **/
Helper::VersionsInformation::value_type qtIconResourcesVersion()
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

static void Resources_initialise()
{
  Q_INIT_RESOURCE( font_awesome );
}
