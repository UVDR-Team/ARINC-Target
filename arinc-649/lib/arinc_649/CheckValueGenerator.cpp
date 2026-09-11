// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc649::CheckValueGenerator.
 **/

#include "CheckValueGenerator.hpp"

#include <arinc_649/Arinc649Exception.hpp>
#include <arinc_649/CheckValue.hpp>
#include <arinc_649/CheckValueTypeDescription.hpp>

#include <arinc_649/implementation/CrcCheckValueGenerator.hpp>
#include <arinc_649/implementation/HashCheckValueGenerator.hpp>
#include <arinc_649/implementation/NopCheckValueGenerator.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/version.hpp>

#if BOOST_VERSION >= 108800
#define ARINC_649_HAVE_BOOST_HASH2 1
#include <boost/hash2/md5.hpp>
#include <boost/hash2/sha1.hpp>
#include <boost/hash2/sha2.hpp>
#endif

#include <boost/exception/all.hpp>

#include <fstream>
#include <vector>

namespace Arinc649 {

std::optional< CheckValue > CheckValueGenerator::checkValue( const CheckValueType type, Helper::ConstRawDataSpan data )
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
    BOOST_THROW_EXCEPTION( Arinc649Exception()
      << Helper::AdditionalInfo{
        std::format(
          "No check value generator for check value '{}'",
          CheckValueTypeDescription::instance().name( checkValueType ) ) } );
  }

  // check the existence of the given file
  if ( !std::filesystem::is_regular_file( file ) )
  {
    BOOST_THROW_EXCEPTION( Arinc649Exception()
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ file.string() } );
  }

  Helper::RawData data( 4096 );

  // load file
  std::ifstream fileStream{ file, std::ifstream::binary | std::ifstream::in };

  if ( !fileStream.is_open() )
  {
    BOOST_THROW_EXCEPTION( Arinc649Exception()
      << Helper::AdditionalInfo{ "Error opening file" }
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
        Arinc649Exception()
          << Helper::AdditionalInfo{ "Error reading file" }
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
#if ARINC_649_HAVE_BOOST_HASH2
      return std::make_shared< HashCheckValueGenerator< Md5, boost::hash2::md5_128 > >();
#else
      SPDLOG_ERROR( "MD5 check value generator requires Boost >= 1.88 (Boost.Hash2)" );
      return {};
#endif

    case Sha1:
#if ARINC_649_HAVE_BOOST_HASH2
      return std::make_shared< HashCheckValueGenerator< Sha1, boost::hash2::sha1_160 > >();
#else
      SPDLOG_ERROR( "SHA1 check value generator requires Boost >= 1.88 (Boost.Hash2)" );
      return {};
#endif

    case Sha256:
#if ARINC_649_HAVE_BOOST_HASH2
      return std::make_shared< HashCheckValueGenerator< Sha256, boost::hash2::sha2_256 > >();
#else
      SPDLOG_ERROR( "SHA256 check value generator requires Boost >= 1.88 (Boost.Hash2)" );
      return {};
#endif

    case Sha512:
#if ARINC_649_HAVE_BOOST_HASH2
      return std::make_shared< HashCheckValueGenerator< Sha512, boost::hash2::sha2_512 > >();
#else
      SPDLOG_ERROR( "SHA512 check value generator requires Boost >= 1.88 (Boost.Hash2)" );
      return {};
#endif

    case Crc64:
      return std::make_shared< Crc64CheckValueGenerator >();

    default:
      SPDLOG_ERROR( "Invalid generator" );
      return {};
  }
}

}
