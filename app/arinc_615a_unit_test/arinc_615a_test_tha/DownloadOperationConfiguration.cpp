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
 * @brief Definition of Class DownloadOperationConfiguration.
 **/

#include "DownloadOperationConfiguration.hpp"

#include <arinc_checksum/CheckValueTypeDescription.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Arinc615aTha {

DownloadOperationConfiguration::DownloadOperationConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void DownloadOperationConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  enabled = properties.get( "enabled", enabled );

  if ( auto directoriesProperty{ properties.get_child_optional( "directories" ) }; directoriesProperty )
  {
    for ( const auto &[ key, value ] : *directoriesProperty )
    {
      if ( key != "directory" )
      {
        // Skip entries not defining a directory
        continue;
      }

      directories.emplace_back( value.get_value< std::filesystem::path >() );
    }
  }

  partNumberOption = properties.get( "part_number_option", partNumberOption );

  checksumOption = ArincChecksum::CheckValueTypeDescription::instance()
    .enumeration( properties.get< std::string >( "checksum_option", {} ) )
    .value_or( checksumOption );
}

boost::property_tree::ptree DownloadOperationConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties;

  properties.add( "enabled", enabled );

  if ( full || !directories.empty() )
  {
    auto directoriesProperty{ properties.add_child( "directories", {} ) };
    for ( const auto &directory : directories )
    {
      directoriesProperty.add( "directory", directory.string() );
    }
  }

  if ( full || partNumberOption )
  {
    properties.add( "part_number_option", partNumberOption );
  }

  if ( full || ( ArincChecksum::CheckValueType::NotUsed != checksumOption ) )
  {
    properties.add( "checksum_option", ArincChecksum::CheckValueTypeDescription::instance().name( checksumOption ) );
  }

  return properties;
}

}
