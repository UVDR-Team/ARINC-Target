// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::FileInfo.
 **/

#include "FileInfo.hpp"

#include <algorithm>

namespace Arinc665::Files {

std::filesystem::path FileInfo::path() const
{
  std::string newPathName{ pathName };

  std::ranges::replace( newPathName, '\\', '/' );

  return ( std::filesystem::path{ newPathName } / filename).make_preferred();
}

bool FileInfo::operator ==( const FileInfo &other ) const
{
  return ( filename == other.filename )
    && ( pathName == other.pathName )
    && ( memberSequenceNumber == other.memberSequenceNumber )
    && ( crc == other.crc )
    && ( checkValue == other.checkValue );
}

}
