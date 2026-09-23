// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::FileTypeDescription.
 **/

#include "FileTypeDescription.hpp"

#include <ostream>

namespace Arinc665 {

FileTypeDescription::FileTypeDescription():
  Description{
    { "BatchFile",        FileType::BatchFile },
    { "LoadUploadHeader", FileType::LoadUploadHeader },
    { "LoadList",         FileType::LoadList },
    { "BatchList",        FileType::BatchList },
    { "FileList",         FileType::FileList },
  }
{
}

std::ostream& operator<<( std::ostream &stream, const FileType fileType )
{
  return ( stream << FileTypeDescription::instance().name( fileType ) );
}

}
