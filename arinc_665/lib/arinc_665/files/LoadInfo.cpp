// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::LoadInfo.
 **/

#include "LoadInfo.hpp"

#include <arinc_665/files/FileInfo.hpp>

namespace Arinc665::Files {

bool LoadInfo::operator ==( const LoadInfo &other ) const
{
  return ( partNumber == other.partNumber )
    && ( headerFilename == other.headerFilename )
    && ( memberSequenceNumber == other.memberSequenceNumber )
    && ( targetHardwareIds == other.targetHardwareIds );
}

bool LoadInfo::operator==( const FileInfo &other ) const
{
  return ( headerFilename == other.filename )
    && ( memberSequenceNumber == other.memberSequenceNumber );
}

bool operator==( const FileInfo &fileInfo, const LoadInfo &loadInfo )
{
  // delegate to class member compare operator
  return loadInfo == fileInfo;
}

}
