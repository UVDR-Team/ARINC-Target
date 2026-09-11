// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module KnownDirectories.
 **/

#include <helper/KnownDirectories.hpp>

#include <cstdlib>

namespace Helper {

/**
 * @brief Returns the XDG Directory from the environment
 *
 * @param[in] xdgEnv
 *   Environment variable
 * @param[in] defaultAdditionalPath
 *   Default path, when environment variable is not set
 *
 * @return The XDG known directory.
 **/
static std::filesystem::path KnownDirectories_xdgDirectory(
  char const * xdgEnv,
  const std::filesystem::path &defaultAdditionalPath );

std::filesystem::path KnownDirectories_home()
{
  static std::filesystem::path home;

  if ( home.empty() )
  {
    if ( auto const * const pathEnv{ std::getenv( "HOME" ) }; nullptr != pathEnv )
    {
      home = pathEnv;
    }
  }

  return home;
}

std::filesystem::path KnownDirectories_dataHome()
{
  static const auto dataHome{ KnownDirectories_xdgDirectory( "XDG_DATA_HOME", ".local/share" ) };
  return dataHome;
}

std::filesystem::path KnownDirectories_configHome()
{
  static const auto configHome{ KnownDirectories_xdgDirectory( "XDG_CONFIG_HOME", ".config" ) };
  return configHome;
}

std::filesystem::path KnownDirectories_stateHome()
{
  static const auto stateHome{ KnownDirectories_xdgDirectory( "XDG_STATE_HOME", ".local/state" ) };
  return stateHome;
}

std::filesystem::path KnownDirectories_cacheHome()
{
  static const auto cacheHome{ KnownDirectories_xdgDirectory( "XDG_CACHE_HOME", ".cache" ) };
  return cacheHome;
}

static std::filesystem::path KnownDirectories_xdgDirectory(
  char const * const xdgEnv,
  const std::filesystem::path &defaultAdditionalPath )
{
  const auto * const xdgDirectory{ std::getenv( xdgEnv ) };
  return ( nullptr != xdgDirectory )
    ? xdgDirectory
    : KnownDirectories_home() / defaultAdditionalPath;
}

}
