// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::LoadInfo.
 **/

#ifndef ARINC_665_FILES_LOADINFO_HPP
#define ARINC_665_FILES_LOADINFO_HPP

#include <arinc_665/files/Files.hpp>

#include <arinc_665/MediumNumber.hpp>

#include <string>
#include <list>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 Load Information.
 *
 * Load Information stored within the load list of the ARINC 665 Loads List File.
 *
 * @sa LoadListFile
 **/
struct ARINC_665_EXPORT LoadInfo
{
  //! Target Hardware IDs.
  using ThwIds = std::list< std::string >;

  //! Load Part Number
  std::string partNumber;
  //! Header Filename
  std::string headerFilename;
  //! Member Sequence Number
  MediumNumber memberSequenceNumber;
  //! Target Hardware IDs (list)
  ThwIds targetHardwareIds;

  /**
   * @brief Compares the other load information against @p this for equality.
   *
   * @param[in] other
   *   Other load information
   *
   * @return if @p this and @p other are equal.
   **/
  bool operator ==( const LoadInfo &other ) const;

  /**
   * @brief Compares the other file information against @p this for equality.
   *
   * @param[in] other
   *   Other file information
   *
   * @return if @p this and @p other are equal.
   **/
  bool operator ==( const FileInfo &other ) const;
};

/**
 * @brief Compares @p FileInfo against @p LoadInfo
 *
 * @param[in] fileInfo
 *   File Information.
 * @param[in] loadInfo
 *   Load Information.
 *
 * @return If file information is equal to the load information.
 **/
bool operator ==( const FileInfo &fileInfo, const LoadInfo &loadInfo );

}

#endif
