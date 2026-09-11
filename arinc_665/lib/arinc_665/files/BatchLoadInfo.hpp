// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::BatchLoadInfo.
 **/

#ifndef ARINC_665_FILES_BATCHLOADINFO_HPP
#define ARINC_665_FILES_BATCHLOADINFO_HPP

#include <arinc_665/files/Files.hpp>

#include <string>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 Batch Load Information.
 *
 * Load Information stored within the target list of the ARINC 665 Batch File.
 *
 * @sa BatchFile
 * @sa BatchTargetInfo
 **/
struct ARINC_665_EXPORT BatchLoadInfo
{
  //! Load Header Filename.
  std::string headerFilename;
  //! Load Part Number.
  std::string partNumber;
};

}

#endif
