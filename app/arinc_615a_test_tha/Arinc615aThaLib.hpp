// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 *
 * @brief C++ API for ARINC 615A Target Hardware Application (THA).
 **/

#ifndef ARINC_615A_THA_LIB_HPP
#define ARINC_615A_THA_LIB_HPP

#include "TargetDataLoaderConfiguration.hpp"
#include "arinc_615a_tha.h"

#include <boost/property_tree/ptree_fwd.hpp>

#include <string>
#include <string_view>
#include <filesystem>

namespace Arinc615aTha {

/**
 * @brief Initialize the THA Target using a boost::property_tree::ptree.
 *
 * @param[in] properties
 *   Configuration property tree.
 *
 * @return 0 on success, negative value on error.
 **/
int init( const boost::property_tree::ptree &properties );

/**
 * @brief Initialize the THA Target using a JSON string.
 *
 * @param[in] jsonStr
 *   JSON configuration string.
 *
 * @return 0 on success, negative value on error.
 **/
int initFromJson( std::string_view jsonStr );

/**
 * @brief Initialize the THA Target using a JSON configuration file.
 *
 * @param[in] jsonFilePath
 *   Path to JSON file.
 *
 * @return 0 on success, negative value on error.
 **/
int initFromFile( const std::filesystem::path &jsonFilePath );

/**
 * @brief Initialize the THA Target with default configuration.
 *
 * @return 0 on success, negative value on error.
 **/
int initDefault();

/**
 * @brief Start the ARINC 615A THA Target event loop.
 *
 * @param[in] runInBackground
 *   If true, spawns a worker thread and returns immediately.
 *   If false, blocks until stop() is called.
 *
 * @return 0 on success, negative value on error.
 **/
int start( bool runInBackground = false );

/**
 * @brief Stop the ARINC 615A THA Target gracefully.
 **/
void stop();

/**
 * @brief Check if the THA Target is currently running.
 *
 * @return True if running, false if stopped.
 **/
bool isRunning();

/**
 * @brief Get the currently active configuration.
 *
 * @return A snapshot of the configuration. Reconfigure through init() while stopped.
 **/
TargetDataLoaderConfiguration configuration();

} // namespace Arinc615aTha

#endif // ARINC_615A_THA_LIB_HPP
