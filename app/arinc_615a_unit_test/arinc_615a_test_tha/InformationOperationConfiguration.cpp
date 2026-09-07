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
 * @brief Definition of Class InformationOperationConfiguration.
 **/

#include "InformationOperationConfiguration.hpp"

#include <arinc_645/CheckValueTypeDescription.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Arinc615aTha {

InformationOperationConfiguration::InformationOperationConfiguration( const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void InformationOperationConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  enabled = properties.get( "enabled", enabled );

  listCheckValue = Arinc645::CheckValueTypeDescription::instance().enumeration(
    properties.get< std::string >( "list_check_value", {} ) ).value_or( listCheckValue );

  if (
    auto const targetsHardwareProperty{ properties.get_child_optional( "targets_hardware" ) };
    targetsHardwareProperty )
  {
    targetsHardware = Arinc615a::Information::TargetHardware::targetsHardware( *targetsHardwareProperty );
  }
}

boost::property_tree::ptree InformationOperationConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties;

  properties.add( "enabled", enabled );

  if ( full || ( listCheckValue != Arinc645::CheckValueType::NotUsed ) )
  {
    properties.add( "list_check_value", Arinc645::CheckValueTypeDescription::instance().name( listCheckValue ) );
  }

  if ( full || !targetsHardware.empty() )
  {
    properties.add_child(
      "targets_hardware",
      Arinc615a::Information::TargetHardware::targetsHardware( targetsHardware ) );
  }
  return properties;
}

}
