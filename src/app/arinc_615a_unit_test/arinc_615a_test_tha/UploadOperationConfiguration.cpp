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

#include "UploadOperationConfiguration.hpp"

#include <boost/property_tree/ptree.hpp>

namespace Arinc615aTha {

UploadOperationConfiguration::UploadOperationConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void UploadOperationConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  enabled = properties.get( "enabled", enabled );
  directory = properties.get( "directory", directory );
  partNumberOption = properties.get( "part_number_option", partNumberOption );
  checksumOption = properties.get( "checksum_option", checksumOption );
}

boost::property_tree::ptree UploadOperationConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties;

  properties.add( "enabled", enabled );
  if ( full || !directory.empty() )
  {
    properties.add( "directory", directory.string() );
  }

  if ( full || partNumberOption )
  {
    properties.add( "part_number_option", partNumberOption );
  }

  if ( full || checksumOption )
  {
    properties.add( "checksum_option", checksumOption );
  }

  return properties;
}

}
