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
 * @brief Definition of Class OperationConfiguration.
 **/

#include "OperationConfiguration.hpp"

#include <boost/property_tree/ptree.hpp>

namespace Arinc615aTha {

OperationConfiguration::OperationConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void OperationConfiguration::fromProperties(
  const boost::property_tree::ptree &properties )
{
  if (
    const auto informationOperationProperties{ properties.get_child_optional( "information_operation" ) };
    informationOperationProperties )
  {
    informationOperation.fromProperties( *informationOperationProperties );
  }

  if (
    const auto uploadOperationProperties{ properties.get_child_optional( "upload_operation" ) };
    uploadOperationProperties )
  {
    uploadOperation.fromProperties( *uploadOperationProperties );
  }

  if (
    const auto mediaDefinedDownloadOperationProperties{
      properties.get_child_optional( "media_defined_download_operation" ) };
    mediaDefinedDownloadOperationProperties )
  {
    mediaDefinedDownloadOperation.fromProperties( *mediaDefinedDownloadOperationProperties );
  }

  if (
    const auto operatorDefinedDownloadOperationProperties{
      properties.get_child_optional( "operator_defined_download_operation" ) };
    operatorDefinedDownloadOperationProperties )
  {
    operatorDefinedDownloadOperation.fromProperties( *operatorDefinedDownloadOperationProperties );
  }
}

boost::property_tree::ptree OperationConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties;

  properties.add_child( "information_operation", informationOperation.toProperties( full ) );
  properties.add_child( "upload_operation", uploadOperation.toProperties( full ) );
  properties.add_child( "media_defined_download_operation", mediaDefinedDownloadOperation.toProperties( full ) );
  properties.add_child( "operator_defined_download_operation", operatorDefinedDownloadOperation.toProperties( full ) );

  return properties;
}

}
