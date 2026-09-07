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
 * @brief Declaration of Class InformationOperationConfiguration.
 **/

#ifndef INFORMATIONOPERATIONCONFIGURATION_HPP
#define INFORMATIONOPERATIONCONFIGURATION_HPP

#include <arinc_615a/information/TargetHardware.hpp>
#include <arinc_615a/information/PartNumber.hpp>

#include <arinc_645/Arinc645.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

namespace Arinc615aTha {

//! Information Operation Configuration
struct InformationOperationConfiguration
{
  //! Default Constructor
  InformationOperationConfiguration() = default;

  /**
   * @brief Loads the information operation configuration from a boost property tree
   *
   * @param[in] properties
   *   Configuration as property tree.
   **/
  explicit InformationOperationConfiguration( const boost::property_tree::ptree &properties );

  /**
   * @brief Load Configuration from given Property Tree.
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

  //! If operation is enabled.
  bool enabled{ false };
  //! Check Value for List Transfer
  Arinc645::CheckValueType listCheckValue{ Arinc645::CheckValueType::NotUsed };
  //! Target Hardware Information
  Arinc615a::Information::TargetsHardware targetsHardware;
};

}

#endif
