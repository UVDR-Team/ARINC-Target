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
 * @brief Declaration of Class DownloadOperationConfiguration.
 **/

#ifndef ARINC615A_DOWNLOADOPERATIONCONFIGURATION_HPP
#define ARINC615A_DOWNLOADOPERATIONCONFIGURATION_HPP

#include <arinc_checksum/Arinc645.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <filesystem>
#include <list>

namespace Arinc615aTha {

//! Download Operation Configuration (Media and Operator defined)
struct DownloadOperationConfiguration
{
  //! Default Configuration
  DownloadOperationConfiguration() = default;

  /**
   * @brief Loads the download operation configuration from a boost property tree.
   *
   * @param[in] properties
   *   Configuration as property tree.
   **/
  explicit DownloadOperationConfiguration( const boost::property_tree::ptree &properties );

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
  //! Source for Downloadable Files
  std::list< std::filesystem::path > directories;
  //! Provide Part Number
  bool partNumberOption{ false };
  //! Checksum Option enabled (only ARINC 615A-3+)
  ArincChecksum::CheckValueType checksumOption{ ArincChecksum::CheckValueType::NotUsed };
};

}

#endif
