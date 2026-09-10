// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class ArincChecksum::CheckValueGenerator.
 **/

#include "CheckValueGenerator.hpp"

#include <arinc_checksum/Arinc645Exception.hpp>
#include <arinc_checksum/CheckValue.hpp>
#include <arinc_checksum/CheckValueTypeDescription.hpp>

#include <arinc_checksum/implementation/CrcCheckValueGenerator.hpp>
#include <arinc_checksum/implementation/HashCheckValueGenerator.hpp>
#include <arinc_checksum/implementation/NopCheckValueGenerator.hpp>

#include <arinc_support/Exception.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/hash2/md5.hpp>
#include <boost/hash2/sha1.hpp>
#include <boost/hash2/sha2.hpp>

#include <boost/exception/all.hpp>

#include <fstream>
#include <vector>

namespace ArincChecksum {

std::optional< CheckValue > CheckValueGenerator::checkValue( const CheckValueType type, ArincSupport::ConstRawDataSpan data )
{
  const auto generator{ create( type ) };

  if ( !generator )
  {
    return std::nullopt;
  }

  generator->process( data );
  return generator->checkValue();
}

CheckValue CheckValueGenerator::checkValue( const CheckValueType checkValueType, const std::filesystem::path &file )
{
  const auto generator{ create( checkValueType ) };

  if ( !generator )
  {
    BOOST_THROW_EXCEPTION( Arinc645Exception()
      << ArincSupport::AdditionalInfo{
        std::format(
          "No check value generator for check value '{}'",
          CheckValueTypeDescription::instance().name( checkValueType ) ) } );
  }

  // check the existence of the given file
  if ( !std::filesystem::is_regular_file( file ) )
  {
    BOOST_THROW_EXCEPTION( Arinc645Exception()
      << ArincSupport::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ file.string() } );
  }

  ArincSupport::RawData data( 4096 );

  // load file
  std::ifstream fileStream{ file, std::ifstream::binary | std::ifstream::in };

  if ( !fileStream.is_open() )
  {
    BOOST_THROW_EXCEPTION( Arinc645Exception()
      << ArincSupport::AdditionalInfo{ "Error opening file" }
      << boost::errinfo_file_name{ file.string() } );
  }

  while ( !fileStream.eof() )
  {
    // read the data to the buffer
    fileStream.read(
      reinterpret_cast< char * >( std::data( data ) ),
      static_cast< std::streamsize >( std::size( data ) ) );

    if ( fileStream.bad() )
    {
      BOOST_THROW_EXCEPTION(
        Arinc645Exception()
          << ArincSupport::AdditionalInfo{ "Error reading file" }
          << boost::errinfo_file_name{ file.string() } );
    }

    generator->process( { data.begin(), static_cast< std::size_t >( fileStream.gcount() ) } );
  }

  return generator->checkValue();
}

CheckValueGeneratorPtr CheckValueGenerator::create( const CheckValueType type )
{
  switch ( type )
  {
    using enum CheckValueType;

    case NotUsed:
      return std::make_shared< NopCheckValueGenerator >();

    case Crc8:
      return std::make_shared< Crc8CheckValueGenerator >();

    case Crc16:
      return std::make_shared< Crc16CheckValueGenerator >();

    case Crc32:
      return std::make_shared< Crc32CheckValueGenerator >();

    case Md5:
      return std::make_shared< HashCheckValueGenerator< Md5, boost::hash2::md5_128 > >();

    case Sha1:
      return std::make_shared< HashCheckValueGenerator< Sha1, boost::hash2::sha1_160 > >();

    case Sha256:
      return std::make_shared< HashCheckValueGenerator< Sha256, boost::hash2::sha2_256 > >();

    case Sha512:
      return std::make_shared< HashCheckValueGenerator< Sha512, boost::hash2::sha2_512 > >();

    case Crc64:
      return std::make_shared< Crc64CheckValueGenerator >();

    default:
      ARINC_LOG_ERROR( "Invalid generator" );
      return {};
  }
}

}
