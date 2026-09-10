// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc615a::Files::LoadConfigurationListFile
 **/

#include "LoadConfigurationListFile.hpp"

#include <arinc_615a/files/String.hpp>

#include <arinc_615a/information/TargetHardware.hpp>
#include <arinc_615a/information/PartNumber.hpp>

#include <arinc_615a/Arinc615aException.hpp>

#include <arinc_support/Exception.hpp>
#include <arinc_support/RawData.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/throw_exception.hpp>

#include <limits>

namespace Arinc615a::Files {

LoadConfigurationListFile::LoadConfigurationListFile(
  Arinc615aVersion const protocolVersion,
  Information::TargetsHardware targetsHardware ) :
  ProtocolFile{ protocolVersion },
  targetsHardwareV{ std::move( targetsHardware ) }
{
}

LoadConfigurationListFile::LoadConfigurationListFile( ArincSupport::ConstRawDataSpan rawData )
{
  decode( rawData );
}

LoadConfigurationListFile& LoadConfigurationListFile::operator=( ArincSupport::ConstRawDataSpan rawData )
{
  decode( rawData );
  return *this;
}

const Information::TargetsHardware& LoadConfigurationListFile::targetsHardware() const
{
  return targetsHardwareV;
}

Information::TargetsHardware& LoadConfigurationListFile::targetsHardware()
{
  return targetsHardwareV;
}

void LoadConfigurationListFile::targetsHardware( Information::TargetsHardware targetsHardware )
{
  targetsHardwareV = std::move( targetsHardware );
}

ArincSupport::RawData LoadConfigurationListFile::encode() const
{
  ArincSupport::RawData rawData( HeaderSize + 2UZ );

  // skip header - it is filled finally
  auto nextData{ ArincSupport::RawDataSpan{ rawData }.subspan( HeaderSize ) };

  // Number of targets must not exceed field
  if ( targetsHardwareV.size() > std::numeric_limits< uint16_t >::max() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException()
      << ArincSupport::AdditionalInfo{ "More THW IDs than allowed" } );
  }

  // number of target hardware information
  nextData = ArincSupport::RawData_setInt( nextData, static_cast< uint16_t >( targetsHardwareV.size() ) );
  assert( nextData.empty() );

  // targets hardware
  for ( const auto &thw : targetsHardwareV )
  {
    // literal name
    const auto rawTargetHardwareCode{ String_encode( thw.literalName ) };

    // serial number
    const auto rawSerialNumber{ String_encode( thw.serialNumber ) };

    rawData.reserve( rawData.size() + rawTargetHardwareCode.size() + rawSerialNumber.size() + 2U );

    rawData.insert( rawData.end(), rawTargetHardwareCode.begin(), rawTargetHardwareCode.end() );

    rawData.insert( rawData.end(), rawSerialNumber.begin(), rawSerialNumber.end() );

    rawData.resize( rawData.size() + 2U );

    // Number of part numbers must not exceed field
    if ( thw.partNumbers.size() > std::numeric_limits< uint16_t>::max() )
    {
      BOOST_THROW_EXCEPTION( Arinc615aException{} << ArincSupport::AdditionalInfo{ "More Part Numbers than allowed" } );
    }

    // number of part numbers
    ArincSupport::RawData_setInt< uint16_t >(
      ArincSupport::RawDataSpan{ rawData }.last( 2U ),
      static_cast< uint16_t >( thw.partNumbers.size() ) );

    // iterate over part numbers
    for ( const auto & partNumber : thw.partNumbers )
    {
      // part number
      const auto rawPartNumber{ String_encode( partNumber.partNumber ) };

      // amendment
      const auto rawAmendment{ String_encode( partNumber.amendment ) };

      // part designation
      const auto rawPartDesignationText{ String_encode( partNumber.partDesignation ) };

      rawData.reserve( rawData.size() + rawPartNumber.size() + rawAmendment.size() + rawPartDesignationText.size() );

      rawData.insert( rawData.end(), rawPartNumber.begin(), rawPartNumber.end() );

      rawData.insert( rawData.end(), rawAmendment.begin(), rawAmendment.end() );

      rawData.insert( rawData.end(), rawPartDesignationText.begin(), rawPartDesignationText.end() );
    }
  }

  // insert header
  insertHeader( rawData );

  return rawData;
}

void LoadConfigurationListFile::decode( const ArincSupport::ConstRawDataSpan rawData )
{
  // check minimum data size
  if ( rawData.size() < HeaderSize + 13UZ )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << ArincSupport::AdditionalInfo{ "Protocol file to small" } );
  }

  auto remainingData{ decodeHeader( rawData ) };

  // number of targets hardware
  uint16_t targetHardwareCount;
  std::tie( remainingData, targetHardwareCount ) = ArincSupport::RawData_getInt< uint16_t >( remainingData );

  // iterate over targets hardware
  for ( uint16_t targetHardwareIndex{ 0U }; targetHardwareIndex < targetHardwareCount; ++targetHardwareIndex )
  {
    // literal name
    std::string literalName;
    std::tie( remainingData, literalName ) = String_decode( remainingData );
    if ( literalName.empty() )
    {
      ARINC_LOG_WARN( "literal name is empty" );
    }

    // serial number
    std::string serialNumber;
    std::tie( remainingData, serialNumber ) = String_decode( remainingData );
    if ( serialNumber.empty() )
    {
      ARINC_LOG_WARN( "serial number is empty" );
    }

    // number of part numbers
    uint16_t partNumbersCount;
    std::tie( remainingData, partNumbersCount ) = ArincSupport::RawData_getInt< uint16_t>( remainingData );

    Information::PartNumbers partNumbers;

    // iterate over part numbers
    for ( uint16_t index = 0U; index < partNumbersCount; ++index )
    {
      // part number
      std::string partNumber;
      std::tie( remainingData, partNumber ) = String_decode( remainingData );
      if ( partNumber.empty() )
      {
        ARINC_LOG_WARN( "part number is empty" );
      }

      // amendment
      std::string amendment;
      std::tie( remainingData, amendment ) = String_decode( remainingData );

      // part designation
      std::string partDesignation;
      std::tie( remainingData, partDesignation ) = String_decode( remainingData );
      if ( partDesignation.empty() )
      {
        ARINC_LOG_WARN( "part designation is empty" );
      }

      // Add to the part number list
      partNumbers.emplace_back( std::move( partNumber ), std::move( amendment ), std::move( partDesignation ) );
    }

    // add target hardware to list
    targetsHardwareV.emplace_back( std::move( literalName ), std::move( serialNumber ), std::move( partNumbers ) );
  }

  // Final Check for additional data
  if ( !remainingData.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc615aException{} << ArincSupport::AdditionalInfo{ "More data then expected" } );
  }
}

}
