// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::MediaSetDefaults.
 **/

#ifndef ARINC_665_UTILS_MEDIASETDEFAULTS_HPP
#define ARINC_665_UTILS_MEDIASETDEFAULTS_HPP

#include <arinc_665/utils/Utils.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Handling Default Configuration.
 *
 * This class provides defaults for Media Set Handling (loading, importing, and creation).
 * These defaults are runtime-configurable and can be loaded/ stored from boost::property_tree instances.
 *
 * These settings are used for the Media Set Manager.
 **/
class ARINC_665_EXPORT MediaSetDefaults
{
  public:
    //! Default Value for Check File Integrity
    static constexpr bool DefaultCheckFileIntegrity{ true };
    //! Default Value for ARINC 665 Version
    static constexpr auto DefaultVersion{ SupportedArinc665Version::Supplement345 };
    //! Default Value for File Creation Policy
    static constexpr auto DefaultFileCreationPolicy{ FileCreationPolicy::NoneExisting };

    //! Initialises the configuration with default values.
    MediaSetDefaults() = default;

    /**
     * @brief Loads the configuration via a boost::property_tree::ptree.
     *
     * @param[in] properties
     *   Stored configuration.
     **/
    explicit MediaSetDefaults( const boost::property_tree::ptree &properties );

    /**
     * @brief Loads the media set configuration form the given property tree.
     *
     * @param[in] properties
     *   Stored Media Set Manager Configuration.
     **/
    void fromProperties( const boost::property_tree::ptree &properties );

    /**
     * @brief Converts the configuration values to a boost::property_tree::ptree.
     *
     * @param[in] full
     *   If set to true, all options are added to the property tree, even if defaulted.
     *
     * @return Configuration as boost::property_tree::ptree.
     **/
    [[nodiscard]] boost::property_tree::ptree toProperties( bool full = false ) const;

    //! Check File Integrity Default Value
    bool checkFileIntegrity{ DefaultCheckFileIntegrity };
    //! ARINC 665 Default Version (When importing or creating Media Sets)
    SupportedArinc665Version version{ DefaultVersion };
    //! Default Load Header File Create File Policy
    FileCreationPolicy loadHeaderFileCreationPolicy{ DefaultFileCreationPolicy };
    //! Default Load Header File Create File Policy
    FileCreationPolicy batchFileCreationPolicy{ DefaultFileCreationPolicy };
};

}

#endif
