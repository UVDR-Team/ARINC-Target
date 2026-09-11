// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::FileInfo.
 **/

#ifndef ARINC_665_FILES_FILEINFO_HPP
#define ARINC_665_FILES_FILEINFO_HPP

#include <arinc_665/files/Files.hpp>

#include <arinc_665/MediumNumber.hpp>

#include <arinc_649/CheckValue.hpp>

#include <cstdint>
#include <filesystem>
#include <string>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 %File List Information.
 *
 * File Information stored within the file list of the ARINC 665 Files List File.
 *
 * @sa FileListFile
 **/
struct ARINC_665_EXPORT FileInfo
{
  //! Filename
  std::string filename;
  //! Path Name
  std::string pathName;
  //! Member Sequence Number
  MediumNumber memberSequenceNumber;
  //! CRC
  uint16_t crc{};
  //! Check Value (since ARINC 665-3)
  Arinc649::CheckValue checkValue;

  /**
   * @brief Returns the path (path name / filename)
   *
   * @return path (incl. filename)
   **/
  [[nodiscard]] std::filesystem::path path() const;

  /**
   * @brief Compares the given file info against @p this.
   *
   * @param[in] other
   *  Other file info.
   *
   * @return if @p this is equal to @p other.
   **/
  bool operator==( const FileInfo &other ) const;
};

}

#endif
