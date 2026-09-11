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

#include <shlobj.h>

#include <string>

namespace Helper {

/**
 * @brief Encapsulates Windows SHGetKnownFolderPath call.
 *
 * @param[in] folderId
 *   Folder ID to return
 *
 * @return Path representing @p folderId
 **/
static std::wstring KnownDirectories_getKnownWindowsFolder( REFKNOWNFOLDERID folderId );

std::filesystem::path KnownDirectories_home()
{
  return KnownDirectories_getKnownWindowsFolder( FOLDERID_Profile );
}

std::filesystem::path KnownDirectories_dataHome()
{
  return KnownDirectories_getKnownWindowsFolder( FOLDERID_LocalAppData );
}

std::filesystem::path KnownDirectories_configHome()
{
  return KnownDirectories_getKnownWindowsFolder( FOLDERID_LocalAppData );
}

std::filesystem::path KnownDirectories_stateHome()
{
  return KnownDirectories_getKnownWindowsFolder( FOLDERID_LocalAppData );
}

std::filesystem::path KnownDirectories_cacheHome()
{
  return KnownDirectories_getKnownWindowsFolder( FOLDERID_LocalAppData );
}

static std::wstring KnownDirectories_getKnownWindowsFolder( REFKNOWNFOLDERID folderId )
{
  LPWSTR path{ nullptr };
  const auto result{ SHGetKnownFolderPath( folderId, KF_FLAG_DEFAULT, nullptr, &path ) };

  if ( S_OK != result )
  {
    return {};
  }

  std::wstring pathString{ path };

  CoTaskMemFree( path );

  return pathString;
}

}
