// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::Arinc665Xml.
 **/

#include "Arinc665Xml.hpp"

#include <arinc_665/utils/implementation/Arinc665XmlLoadImpl.hpp>
#include <arinc_665/utils/implementation/Arinc665XmlSaveImpl.hpp>

namespace Arinc665::Utils {

LoadXmlResult Arinc665Xml_load( const std::filesystem::path &xmlFile )
{
  Arinc665XmlLoadImpl load{ xmlFile };
  return load();
}

void Arinc665Xml_save(
  const Media::MediaSet &mediaSet,
  const FilePathMapping &filePathMapping,
  const std::filesystem::path &xmlFile )
{
  Arinc665XmlSaveImpl save{ mediaSet, filePathMapping, xmlFile };
  save();
}

}
