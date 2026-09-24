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
 * @brief Declaration of Class UploadOperationConfiguration.
 **/

#ifndef UPLOADOPERATIONCONFIGURATION_HPP
#define UPLOADOPERATIONCONFIGURATION_HPP

#include <boost/property_tree/ptree_fwd.hpp>

#include <filesystem>

namespace Arinc615aTha {

//! Upload Operation Configuration
struct UploadOperationConfiguration
{
  //! Default Constructor
  UploadOperationConfiguration() = default;

  /**
   * @brief Loads the upload operation configuration from a boost
   *   property tree.
   *
   * @param[in] properties
   *   Configuration as property tree.
   **/
  explicit UploadOperationConfiguration( const boost::property_tree::ptree &properties );

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
  //! Target Directory for Uploaded Files
  std::filesystem::path directory{ "." };
  //! Part Number Option enabled (only ARINC 615A-3+)
  bool partNumberOption{ false };
  //! Checksum Option enabled (only ARINC 615A-3+)
  bool checksumOption{ false };
};

}

#endif
