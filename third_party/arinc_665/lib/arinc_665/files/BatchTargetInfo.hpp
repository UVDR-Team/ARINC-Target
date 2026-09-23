// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::BatchTargetInfo.
 **/

#ifndef ARINC_665_FILES_BATCHTARGETINFO_HPP
#define ARINC_665_FILES_BATCHTARGETINFO_HPP

#include <arinc_665/files/Files.hpp>
#include <arinc_665/files/BatchLoadInfo.hpp>

#include <string>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 Batch Target Information.
 *
 * The target hardware information, which is part of the batch file.
 *
 * Within ARINC 665-2 The Target Hardware ID Position Field is named Target HW ID.
 *
 * @sa BatchListFile
 * @sa BatchLoadInfo, BatchLoadsInfo
 **/
struct ARINC_665_EXPORT BatchTargetInfo
{
  //! Target Hardware ID and Position
  std::string targetHardwareIdPosition;
  //! List of Loads for Target Hardware
  BatchLoadsInfo loads;
};

}

#endif
