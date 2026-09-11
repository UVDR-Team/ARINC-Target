// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Arinc649 Filename.
 **/

#ifndef ARINC_649_FILENAME_HPP
#define ARINC_649_FILENAME_HPP

#include <arinc_649/Arinc649.hpp>

#include <string_view>

namespace Arinc649 {

//! Characters, which are not allowed within filenames
constexpr std::string_view FilenameProhibitCharacters{ " *\\/\"<>?|~:'" };

/**
 * @brief Check for a valid filename.
 *
 * This operation checks the given @p filename for validity, according to _ARINC 649_.
 *
 * Checks that @p filename:
 *  - is not *empty*,
 *  - is not `.`,
 *  - is not `..`, and
 *  - does not contain any character of @ref FilenameProhibitCharacters.
 *
 * Allowed Characters:
 * ``* ! # $ % & ' ( ) + , - . 0-9 ; = @ A-Z [ ] ^ _ ` a-z { | } ~``
 *
 * @attention
 * This operation expects a _filename_ not a _file path_!
 *
 * @param[in] filename
 *   Check filename.
 *
 * @return If the parameter @p filename is valid.
 **/
[[nodiscard]] ARINC_649_EXPORT bool Filename_check( std::string_view filename );

}

#endif
