// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Media::RegularFile.
 **/

#include "RegularFile.hpp"

namespace Arinc665::Media {

RegularFile::RegularFile(
  const ContainerEntityPtr &parent,
  std::string name,
  const OptionalMediumNumber &mediumNumber,
  [[maybe_unused]] const CreateKey &createKey ) :
  File{ parent, std::move( name ), mediumNumber }
{
}

FileType RegularFile::fileType() const
{
  return FileType::RegularFile;
}

}
