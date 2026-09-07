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
 * @brief Declaration of Class TargetDataLoaderConfiguration.
 **/

#ifndef TARGETDATALOADERCONFIGURATION_HPP
#define TARGETDATALOADERCONFIGURATION_HPP

#include "OperationConfiguration.hpp"

#include <arinc_615a/find/FindConfiguration.hpp>
#include <arinc_615a/find/TargetInformation.hpp>

#include <arinc_615a/Arinc615aConfiguration.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <list>
#include <map>
#include <string>

namespace Arinc615aTha {

//! ARINC 615A Target Data Loader Configuration
struct TargetDataLoaderConfiguration
{
  //! Targets information type (Target ID to Operation Configuration)
  using TargetsConfiguration = std::map< std::string, OperationConfiguration, std::less<> >;

  /**
   * @brief Loads the targets-configuration
   *
   * @param[in] properties
   *   Configuration as Property Tree
   *
   * @return Targets Configuration.
   **/
  [[nodiscard]] static TargetsConfiguration targetsConfiguration( const boost::property_tree::ptree &properties );

  /**
   * @brief Returns the targets configuration as boost property tree.
   *
   * @param[in] targetsConfiguration
   *   Targets Configuration.
   * @param[in] full
   *   If set to true, all options are added to the property tree, even if defaulted.
   *
   * @return The targets configuration as boost property tree.
   **/
  [[nodiscard]] static boost::property_tree::ptree targetsConfiguration(
    const TargetsConfiguration &targetsConfiguration,
    bool full = false );

  /**
   * @brief Creates a Host Data Loader configuration with default values.
   **/
  TargetDataLoaderConfiguration() = default;

  /**
   * @brief Creates a Host Data Loader configuration with the values given by the property tree.
   *
   * Values, not set by the property tree are filled with default values.
   *
   * @param[in] properties
   *   Property tree.
   **/
  explicit TargetDataLoaderConfiguration( const boost::property_tree::ptree &properties );

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

  //! ARINC 615A Data Loader Configuration
  Arinc615a::Arinc615aConfiguration dataLoader;
  //! ARINC 615A FIND Configuration
  Arinc615a::Find::FindConfiguration find;
  //! ARINC 615A FIND Information.
  Arinc615a::Find::TargetsInformation findInformation;
  //! Targets Configuration
  TargetsConfiguration targets;
  //! ARINC 615A Protocol Version to use
  Arinc615a::Arinc615aVersion version{ Arinc615a::Arinc615aVersion::Arinc615a34 };
  //! Transmission Rate of Status Information.
  std::chrono::seconds statusTransmissionRate{ Arinc615a::DefaultArinc615aDlpTimeout / 2U };
};

}

#endif
