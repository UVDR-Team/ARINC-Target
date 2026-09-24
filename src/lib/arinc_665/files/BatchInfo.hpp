// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::BatchInfo.
 **/

#ifndef ARINC_665_FILES_BATCHINFO_HPP
#define ARINC_665_FILES_BATCHINFO_HPP

#include <arinc_665/files/Files.hpp>

#include <arinc_665/MediumNumber.hpp>

#include <string>
#include <string_view>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 Batch Information.
 *
 * Batch Information stored within the batch list of the ARINC 665 Batch List File.
 *
 * @sa BatchListFile
 **/
struct ARINC_665_EXPORT BatchInfo
{
  //! Part Number
  std::string partNumber;
  //! Filename
  std::string filename;
  //! Member Sequence Number
  MediumNumber memberSequenceNumber;

  /**
   * @brief Compares batch info against each other
   *
   * @param[in] other
   *   Right-hand side, which should be compared with @p this.
   *
   * @return @p this is equal to @p other.
   **/
  bool operator ==( const BatchInfo &other ) const;

  /**
   * @brief Compares batch info against file info.
   *
   * @param[in] other
   *   Right-hand side, which should be compared with @p this.
   *
   * @return @p this is equal to @p other.
   **/
  bool operator ==( const FileInfo &other ) const;

  /**
   * @brief Compares batch info against file info to be unequal.
   *
   * @param[in] other
   *   Right-hand side, which should be compared with @p this.
   *
   * @return @p this is un-equal to @p other.
   **/
  bool operator !=( const FileInfo &other) const;
};

/**
 * @brief Compares batch info against file info.
 *
 * @param fileInfo
 *   File info, which should be compared with @p batchInfo.
 * @param batchInfo
 *   Batch info, which should be compared with @p fileInfo.
 *
 * @return @p fileInfo is equal to @p batchInfo.
 **/
bool operator==( const FileInfo &fileInfo, const BatchInfo &batchInfo );

/**
 * @brief Compares batch info against file info.
 *
 * @param fileInfo
 *   File info, which should be compared with @p batchInfo.
 * @param batchInfo
 *   Batch info, which should be compared with @p fileInfo.
 *
 * @return @p fileInfo is un-equal to @p batchInfo.
 **/
bool operator!=( const FileInfo &fileInfo, const BatchInfo &batchInfo );

}

#endif
