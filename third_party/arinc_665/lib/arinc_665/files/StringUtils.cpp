// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of String Utility Functions.
 **/

#include "StringUtils.hpp"

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_support/Exception.hpp>

#include <boost/exception/all.hpp>

#include <cassert>

namespace Arinc665::Files {

std::tuple< ArincSupport::ConstRawDataSpan, std::string_view > StringUtils_decodeString( ArincSupport::ConstRawDataSpan rawData )
{
  auto remaining{ rawData };

  // string length
  uint16_t stringLength{};
  std::tie( remaining, stringLength ) = ArincSupport::RawData_getInt< uint16_t >( remaining );

  // copy string
  std::string_view string;
  std::tie( remaining, string ) =  ArincSupport::RawData_getString( remaining, stringLength );

  // if string is odd skip filled 0-character
  if ( stringLength % 2 == 1 )
  {
    // check fill-character
    if ( std::byte{ 0 } != remaining.front() )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception{} << ArincSupport::AdditionalInfo{ "Fill character not '0'" } );
    }
    remaining = remaining.subspan( 1 );
  }

  return { remaining, string };
}

ArincSupport::RawData StringUtils_encodeString( std::string_view string )
{
  ArincSupport::RawData rawString;
  rawString.reserve( sizeof( uint16_t ) + string.size()  + ( string.size() % sizeof( uint16_t ) ) );

  // set string length
  rawString.resize( sizeof( uint16_t ) );
  ArincSupport::RawData_setInt< uint16_t >( rawString, ArincSupport::safeCast< uint16_t >( string.size() ) );

  // copy string
  auto stringSpan{ ArincSupport::RawData_asRawData( string ) };
  rawString.insert( rawString.end(), stringSpan.begin(), stringSpan.end() );

  // fill string if it is odd
  if ( string.size() % 2 == 1 )
  {
    rawString.push_back( std::byte{ 0U } );
  }

  return rawString;
}

std::tuple< ArincSupport::ConstRawDataSpan, std::list< std::string > > StringUtils_decodeStrings(
  ArincSupport::ConstRawDataSpan rawData )
{
  // empty strings
  std::list< std::string > strings;

  auto remaining{ rawData };

  // number of strings
  uint16_t numberOfEntries{};
  std::tie( remaining, numberOfEntries ) = ArincSupport::RawData_getInt< uint16_t >( remaining );

  for ( uint16_t index = 0U; index < numberOfEntries; ++index )
  {
    // string
    std::string_view string;
    std::tie( remaining, string ) = StringUtils_decodeString( remaining );
    strings.emplace_back( string );
  }

  return { remaining, strings };
}

ArincSupport::RawData StringUtils_encodeStrings( const std::list< std::string > &strings )
{
  ArincSupport::RawData rawStrings( sizeof( uint16_t ) );

  // set number of strings
  ArincSupport::RawData_setInt< uint16_t >( rawStrings, ArincSupport::safeCast< uint16_t >( strings.size() ) );

  for ( const auto &string : strings )
  {
    auto rawString{ StringUtils_encodeString( string ) };
    assert( rawString.size() % 2 == 0 );

    // append string
    rawStrings.insert( rawStrings.end(), rawString.begin(), rawString.end() );
  }

  return rawStrings;
}

}
