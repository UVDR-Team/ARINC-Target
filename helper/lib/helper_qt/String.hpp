// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module String.
 **/

#ifndef HELPER_QT_STRING_HPP
#define HELPER_QT_STRING_HPP

#include <helper_qt/HelperQt.hpp>

#include <QString>

#include <string_view>

namespace HelperQt {

/**
 * @brief Converts the given std::string_view to QString.
 *
 * @param[in] stringView
 *   String View to convert.
 *
 * @return QString representation of @p stringView.
 **/
[[nodiscard]] HELPER_QT_EXPORT QString toQString( std::string_view stringView );

}

#endif
