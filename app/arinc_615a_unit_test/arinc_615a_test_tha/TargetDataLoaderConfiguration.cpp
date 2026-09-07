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
 * @brief Definition of Class TargetDataLoaderConfiguration.
 **/

#include "TargetDataLoaderConfiguration.hpp"

#include <arinc_615a/Arinc615aVersionDescription.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Arinc615aTha {

TargetDataLoaderConfiguration::TargetsConfiguration TargetDataLoaderConfiguration::targetsConfiguration(
  const boost::property_tree::ptree &properties )
{
  TargetsConfiguration targetsConfiguration;

  for ( const auto &[ key, value ] : properties )
  {
    targetsConfiguration.try_emplace( value.get< std::string >( "target_id" ), value );
  }

  return targetsConfiguration;
}

boost::property_tree::ptree TargetDataLoaderConfiguration::targetsConfiguration(
  const TargetsConfiguration &targetsConfiguration,
  const bool full )
{
  boost::property_tree::ptree properties{};

  for ( const auto &[ targetId, targetConfiguration ] : targetsConfiguration )
  {
    boost::property_tree::ptree ptreeConfig;

    ptreeConfig.add( "target_id", targetId );

    const auto tcProperties{ targetConfiguration.toProperties( full ) };
    ptreeConfig.insert( ptreeConfig.end(), tcProperties.begin(), tcProperties.end() );

    properties.add_child( "target_configuration", ptreeConfig );
  }

  return properties;
}

TargetDataLoaderConfiguration::TargetDataLoaderConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void TargetDataLoaderConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  if ( const auto dataLoaderProperties{ properties.get_child_optional( "arinc_615a" ) }; dataLoaderProperties )
  {
    dataLoader.fromProperties( *dataLoaderProperties );
  }

  if ( const auto findProperties{ properties.get_child_optional( "arinc_615a_find" ) }; findProperties )
  {
    find.fromProperties( *findProperties );
  }

  if (
    const auto findInformationProperties{ properties.get_child_optional( "find_information" ) };
    findInformationProperties )
  {
    findInformation = Arinc615a::Find::TargetInformation::targetsInformation( *findInformationProperties );
  }

  if (
    const auto targetsProperties{ properties.get_child_optional( "targets_configuration" ) };
    targetsProperties )
  {
    targets = targetsConfiguration( *targetsProperties );
  }

  version = properties.get( "version", version );

  statusTransmissionRate = properties.get_optional< std::chrono::seconds::rep >( "status_transmission_rate" )
    .map( []( const auto transmissionRate ) { return std::chrono::seconds{ transmissionRate }; } )
    .value_or( statusTransmissionRate );
}

boost::property_tree::ptree TargetDataLoaderConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties;

  properties.add_child( "arinc_615a", dataLoader.toProperties( full ) );

  properties.add_child( "arinc_615a_find", find.toProperties( full ) );

  properties.add_child( "find_information", Arinc615a::Find::TargetInformation::targetsInformation( findInformation ) );

  properties.add_child( "targets_configuration", targetsConfiguration( targets ) );

  if ( full || ( Arinc615a::Arinc615aVersion::Arinc615a34 != version ) )
  {
    properties.add( "version", version );
  }

  if ( full || ( statusTransmissionRate != Arinc615a::DefaultArinc615aDlpTimeout / 2U ) )
  {
    properties.add( "status_transmission_rate", statusTransmissionRate.count() );
  }

  return properties;
}

}
