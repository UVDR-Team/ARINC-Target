// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Filenames.
 **/

#ifndef HELPER_FILENAMES_HPP
#define HELPER_FILENAMES_HPP

#include <helper/Helper.hpp>

#include <string>
#include <string_view>

namespace Helper {

/**
 * @name Filename Handling
 *
 * @sa https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
 *
 * @{
 **/

/**
 * @brief Normalises a given filename by ensuring it complies with certain restrictions and formats.
 *
 * The function performs the following transformations:
 * - If the filename is empty, it returns a default value of `_`.
 * - If the filename is `.` or `..`, it returns `_` or `__` respectively.
 * - If the filename matches any reserved system filenames (like `CON`, `NUL`, etc.), as defined by the
 *   _illegalFilenames_ array, prefixes an underscore (`_`) to the filename.
 * - Replace any characters in the filename that belong to the set of illegal characters (such as `<`, `>`, `:`, etc.,
 *   defined by _illegalCharacters_) with underscores (`_`).
 *
 * This function ensures that the resulting filename is safe to use across different operating systems and avoids naming
 * conflicts or illegal file paths.
 *
 * @param[in] filename
 *   Filename to normalise.
 *
 * @return A normalised string that is valid and safe to use as a filename.
 **/
[[nodiscard]] HELPER_EXPORT std::string normaliseFilename( std::string_view filename );

/** @} **/

}

#endif
