// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module KnownDirectories.
 **/

#ifndef HELPER_KNOWNDIRECTORIES_HPP
#define HELPER_KNOWNDIRECTORIES_HPP

#include <helper/Helper.hpp>

#include <filesystem>

namespace Helper {

/**
 * @name Known Directories
 * @{
 **/

/**
 * @brief Returns the Home-Directory of the current user.
 *
 * @return Home-Directory
 **/
[[nodiscard]] HELPER_EXPORT std::filesystem::path KnownDirectories_home();

/**
 * @brief Directory where user-specific data is written to.
 *
 * @return Directory where user-specific data is written to.
 **/
[[nodiscard]] HELPER_EXPORT std::filesystem::path KnownDirectories_dataHome();

/**
 * @brief Directory where user-specific configuration files are written to.
 *
 * @return Directory where user-specific configuration files are written to.
 **/
[[nodiscard]] HELPER_EXPORT std::filesystem::path KnownDirectories_configHome();

/**
 * @brief Directory where user-specific state data is written to.
 *
 * @return Directory where user-specific state data is written to.
 **/
[[nodiscard]] HELPER_EXPORT std::filesystem::path KnownDirectories_stateHome();

/**
 * @brief Directory where non-essential (cached) data is written to.
 *
 * @return Directory where non-essential (cached) data is written to.
 **/
[[nodiscard]] HELPER_EXPORT std::filesystem::path KnownDirectories_cacheHome();

/** @} **/

}

#endif
