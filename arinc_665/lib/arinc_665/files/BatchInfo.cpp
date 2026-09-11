// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::BatchInfo.
 **/

#include "BatchInfo.hpp"

#include <arinc_665/files/FileInfo.hpp>

namespace Arinc665::Files {

bool BatchInfo::operator ==( const BatchInfo &other) const
{
  return
    ( partNumber == other.partNumber ) &&
    ( filename == other.filename ) &&
    ( memberSequenceNumber == other.memberSequenceNumber );
}

bool BatchInfo::operator ==( const FileInfo &other) const
{
  return
    ( filename == other.filename ) &&
    ( memberSequenceNumber == other.memberSequenceNumber );
}

bool BatchInfo::operator !=( const FileInfo &other) const
{
  return !(*this == other);
}

bool operator ==( const FileInfo &fileInfo, const BatchInfo &batchInfo)
{
  return batchInfo == fileInfo;
}

bool operator !=( const FileInfo &fileInfo, const BatchInfo &batchInfo)
{
  return batchInfo != fileInfo;
}

}
