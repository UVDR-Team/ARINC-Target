// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::MediaSetManagerConfiguration.
 **/

#ifndef ARINC_665_UTILS_MEDIASETMANAGERCONFIGURATION_HPP
#define ARINC_665_UTILS_MEDIASETMANAGERCONFIGURATION_HPP

#include <arinc_665/utils/Utils.hpp>

#include <arinc_665/utils/MediaSetDefaults.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <list>
#include <utility>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Manager Configuration.
 *
 * It can load and store configuration values using the boost::property_tree.
 **/
class ARINC_665_EXPORT MediaSetManagerConfiguration
{
  public:
    //! Media Sets Paths ( List of Media Set Path, @ref MediaSetPaths)
    using MediaSetsPaths = std::list< MediaSetPaths >;

    //! Initialises the configuration with default values.
    MediaSetManagerConfiguration() = default;

    /**
     * @brief Loads the configuration via a boost::property_tree::ptree.
     *
     * @param[in] properties
     *   Stored configuration.
     **/
    explicit MediaSetManagerConfiguration( const boost::property_tree::ptree &properties );

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

    //! Media Sets Paths
    MediaSetsPaths mediaSets;
    //! Media Set Defaults Configuration
    MediaSetDefaults defaults;
};

}

#endif
