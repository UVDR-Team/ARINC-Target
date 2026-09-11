// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module String.
 **/

#include "String.hpp"

namespace HelperQt {

QString toQString( std::string_view stringView )
{
  return QString::fromUtf8(
    stringView.data(),
    static_cast< int >( stringView.size() ) );
}

}
