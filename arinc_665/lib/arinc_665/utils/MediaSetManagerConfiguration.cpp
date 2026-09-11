// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::MediaSetManagerConfiguration.
 **/

#include "MediaSetManagerConfiguration.hpp"

#include <boost/property_tree/ptree.hpp>

namespace Arinc665::Utils {

MediaSetManagerConfiguration::MediaSetManagerConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void MediaSetManagerConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  // iterate over media sets
  if ( const auto mediaSetProperties{ properties.get_child_optional( "media_sets" ) }; mediaSetProperties )
  {
    for ( const auto &[mediaSetPropertyName, mediaSetConfig] : *mediaSetProperties )
    {
      auto mediaSetPath{ mediaSetConfig.get< std::filesystem::path >( "path", {} ) };

      MediaPaths mediaPaths{};

      // iterate over media
      if ( const auto mediaConfigs{ mediaSetConfig.get_child_optional( "media" ) }; mediaConfigs )
      {
        for ( const auto &[ mediaPropertyName, mediumConfig ] : *mediaConfigs )
        {
          auto mediumNumber{ mediumConfig.get< unsigned int >( "number" ) };
          auto mediumPath{ mediumConfig.get< std::filesystem::path >( "path" ) };

          mediaPaths.try_emplace( MediumNumber{ static_cast< uint8_t >( mediumNumber ) }, std::move( mediumPath ) );
        }
      }

      // insert media set configuration
      mediaSets.emplace_back( std::move( mediaSetPath ), std::move( mediaPaths ) );
    }
  }

  defaults = MediaSetDefaults{};
  if ( const auto defaultsProperties{ properties.get_child_optional( "defaults" ) }; defaultsProperties )
  {
    defaults = MediaSetDefaults{ *defaultsProperties };
  }
}

boost::property_tree::ptree MediaSetManagerConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties{};

  // Media Sets configuration
  if ( full || !mediaSets.empty() )
  {
    auto &mediaSetsProperties{ properties.add_child( "media_sets", {} ) };

    for ( const auto &[ mediaSetPath, media ] : mediaSets )
    {
      auto &mediaSetConfig{ mediaSetsProperties.add_child( "media_set", {} ) };

      mediaSetConfig.add( "path", mediaSetPath.string() );

      auto &mediaConfig{ mediaSetConfig.add_child( "media", {} ) };

      for ( const auto &[ mediumNumber, mediumPath ] : media )
      {
        auto &mediumConfig{ mediaConfig.add_child( "medium", {} ) };

        mediumConfig.add( "number", static_cast< uint8_t >( mediumNumber ) );
        mediumConfig.add( "path", mediumPath.string() );
      }
    }
  }

  // defaults
  if ( const auto defaultProperties{ defaults.toProperties( full ) }; full || !defaultProperties.empty() )
  {
    properties.add_child( "defaults", defaultProperties );
  }

  return properties;
}

}
