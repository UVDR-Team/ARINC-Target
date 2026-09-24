// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @ingroup arinc_615a_test_tha
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class OperationConfiguration.
 **/

#ifndef OPERATIONCONFIGURATION_HPP
#define OPERATIONCONFIGURATION_HPP

#include "InformationOperationConfiguration.hpp"
#include "UploadOperationConfiguration.hpp"
#include "DownloadOperationConfiguration.hpp"

#include <boost/property_tree/ptree_fwd.hpp>

namespace Arinc615aTha {

/**
 * @brief Target Configurations
 *
 * - Information Operation,
 * - Media Defined Download Operation,
 * - Operator Defined Download Operation,
 * - Upload Operation
 **/
struct OperationConfiguration
{
  /**
   * @brief Load the Operation Configuration from boost properties.
   *
   * @param[in] properties
   *   The property tree.
   */
  explicit OperationConfiguration( const boost::property_tree::ptree &properties );

  /**
   * @brief Load Configuration from given property tree.
   *
   * @param[in] properties
   *   Configuration as property tree.
   **/
  void fromProperties( const boost::property_tree::ptree &properties );

  /**
   * @brief Returns the configuration as property tree.
   *
   * @param[in] full
   *   If set to true, all options are added to the property tree, even if defaulted.
   *
   * @return Configuration as property tree.
   **/
  [[nodiscard]] boost::property_tree::ptree toProperties( bool full = false ) const;

  //! Information Operation Configuration
  InformationOperationConfiguration informationOperation;
  //! Upload Operation Configuration
  UploadOperationConfiguration uploadOperation;
  //! Media Defined Download Operation Configuration
  DownloadOperationConfiguration mediaDefinedDownloadOperation;
  //! Operator Defined Download Operation Configuration
  DownloadOperationConfiguration operatorDefinedDownloadOperation;
};

}

#endif
