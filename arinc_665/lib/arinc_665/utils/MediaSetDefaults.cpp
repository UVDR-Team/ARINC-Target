// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::MediaSetDefaults.
 **/

#include "MediaSetDefaults.hpp"

#include <arinc_665/utils/FileCreationPolicyDescription.hpp>

#include <arinc_665/SupportedArinc665VersionDescription.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Arinc665::Utils {

constexpr bool MediaSetDefaults::DefaultCheckFileIntegrity;

constexpr SupportedArinc665Version MediaSetDefaults::DefaultVersion;

constexpr FileCreationPolicy MediaSetDefaults::DefaultFileCreationPolicy;

MediaSetDefaults::MediaSetDefaults( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void MediaSetDefaults::fromProperties( const boost::property_tree::ptree &properties )
{
  checkFileIntegrity = properties.get( "check_file_integrity", DefaultCheckFileIntegrity );

  version = properties.get( "version", DefaultVersion );

  loadHeaderFileCreationPolicy = properties.get( "load_header_file_creation_policy", DefaultFileCreationPolicy );
  batchFileCreationPolicy = properties.get( "batch_file_creation_policy", DefaultFileCreationPolicy );
}

boost::property_tree::ptree MediaSetDefaults::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties{};

  if ( full || ( checkFileIntegrity != DefaultCheckFileIntegrity ) )
  {
    properties.add( "check_file_integrity", checkFileIntegrity );
  }

  if ( full || ( version != DefaultVersion ) )
  {
    properties.add( "version", version );
  }

  if ( full || ( loadHeaderFileCreationPolicy != DefaultFileCreationPolicy ) )
  {
    properties.add( "header_file_creation_policy", loadHeaderFileCreationPolicy );
  }

  if ( full || ( batchFileCreationPolicy != DefaultFileCreationPolicy ) )
  {
    properties.add( "batch_file_creation_policy", batchFileCreationPolicy );
  }

  return properties;
}

}
