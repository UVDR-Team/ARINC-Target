// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc649 Filename.
 **/

#include "Filename.hpp"

namespace Arinc649 {

bool Filename_check( const std::string_view filename )
{
  if ( filename.empty() || ( filename == "." ) || ( filename == ".." ) )
  {
    return false;
  }

  return std::string_view::npos == filename.find_first_of( FilenameProhibitCharacters );
}

}
