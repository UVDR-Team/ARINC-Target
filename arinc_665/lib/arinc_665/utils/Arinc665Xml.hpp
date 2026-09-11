// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::Arinc665Xml.
 **/

#ifndef ARINC_665_XML_ARINC665XML_HPP
#define ARINC_665_XML_ARINC665XML_HPP

#include <arinc_665/utils/Utils.hpp>
#include <arinc_665/media/Media.hpp>

#include <filesystem>
#include <map>

namespace Arinc665::Utils {

/**
 * @name ARINC 665 %Media Set XML Representation Handling.
 *
 * Handles the representation of %Media Sets as an XML file.
 *
 * The ARINC 665 XML is an XML-Schema, which can be used to describe ARINC 665 %Media Sets and loads in a human-readable
 * manner.
 *
 * Tools can use this representation to "decompile" binary media sets to this representation and "compile" the XML
 * representation to a binary media set.
 *
 * @{
 **/

//! %Load XML Result (%Media Set Pointer and corresponding %File Path Mapping)
using LoadXmlResult = std::tuple< Media::MediaSetPtr, FilePathMapping >;

/**
 * @brief %Loads the %Media Set information from the given XML %File.
 *
 * @param[in] xmlFile
 *   ARINC 665 XML file.
 *
 * @return Load %Media Set information.
 *
 * @throw Arinc665Exception
 *   If the file cannot be loaded or is invalid.
 **/
[[nodiscard]] ARINC_665_EXPORT LoadXmlResult Arinc665Xml_load( const std::filesystem::path &xmlFile );

/**
 * @brief Saves the given %Media Set Information to the given XML %File.
 *
 * @param[in] mediaSet
 *   %Media Set Information.
 * @param[in] filePathMapping
 *   %File Path Mapping.
 *   Used to insert the correct source path attribute.
 * @param[in] xmlFile
 *   ARINC 665 XML file.
 **/
ARINC_665_EXPORT void Arinc665Xml_save(
  const Media::MediaSet &mediaSet,
  const FilePathMapping &filePathMapping,
  const std::filesystem::path &xmlFile );

/** @} **/

}

#endif
