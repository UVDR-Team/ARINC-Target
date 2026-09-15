// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Routines for Raw Data Handling.
 **/

#include <helper/SafeCast.hpp>

#include <boost/endian/conversion.hpp>

#include <boost/exception/all.hpp>

#include <algorithm>
#include <cassert>
#include <exception>
#include <cstring>

namespace Helper {

template< std::integral IntT, boost::endian::order RawOrder >
[[nodiscard]] constexpr std::tuple< ConstRawDataSpan, IntT > RawData_getInt( ConstRawDataSpan raw )
{
  // Check minimum size
  if ( raw.size() < sizeof( IntT ) )
  {
    BOOST_THROW_EXCEPTION( std::out_of_range( "RawData_getInt: not enough data" ) ) ;
  }

  // convert to native endian
  auto value{ boost::endian::endian_load< IntT, sizeof( IntT ), RawOrder >(
    reinterpret_cast< const unsigned char * >( raw.data() ) ) };

  // return remaining data and value
  return { raw.subspan( sizeof( IntT ) ), value };
}

template< std::integral IntT >
[[nodiscard]] constexpr std::tuple< ConstRawDataSpan, IntT > RawData_getInt(
  ConstRawDataSpan raw,
  const boost::endian::order rawOrder )
{
  // Check minimum size
  if ( raw.size() < sizeof( IntT ) )
  {
    BOOST_THROW_EXCEPTION( std::out_of_range( "RawData_getInt: not enough data" ) ) ;
  }

  // get raw value
  IntT rawValue;
  std::memcpy(&rawValue, raw.data(), sizeof(rawValue));

  // convert to native endian
  const auto value{ boost::endian::conditional_reverse( rawValue, rawOrder, boost::endian::order::native ) };

  // return remaining data and value
  return { raw.subspan( sizeof( IntT ) ), value };
}

template< std::integral IntT, boost::endian::order RawOrder >
constexpr RawDataSpan RawData_setInt( RawDataSpan raw, const IntT value )
{
  // Check minimum size
  if ( raw.size() < sizeof( IntT ) )
  {
    BOOST_THROW_EXCEPTION( std::out_of_range( "RawData_setInt: not enough data" ) ) ;
  }

  boost::endian::endian_store< IntT, sizeof( IntT ), RawOrder >(
    reinterpret_cast< unsigned char * >( raw.data() ),
    value );

  return raw.subspan( sizeof( IntT ) );
}

template< std::integral IntT >
constexpr RawDataSpan RawData_setInt( RawDataSpan raw, IntT value, const boost::endian::order rawOrder )
{
  // Check minimum size
  if ( raw.size() < sizeof( IntT ) )
  {
    BOOST_THROW_EXCEPTION( std::out_of_range( "RawData_setInt: not enough data" ) ) ;
  }

  // convert to raw endian
  const auto rawValue = boost::endian::conditional_reverse( value, boost::endian::order::native, rawOrder );
  std::memcpy(raw.data(), &rawValue, sizeof(rawValue));

  return raw.subspan( sizeof( IntT ) );
}

template< std::integral IntT, boost::endian::order RawOrder >
[[nodiscard]] constexpr std::array< std::byte, sizeof( IntT ) > RawData_toRaw( const IntT value )
{
  std::array< std::byte, sizeof( IntT ) > result;
  [[maybe_unused]] auto remaining{ RawData_setInt< IntT, RawOrder >( result, value ) };
  assert( remaining.empty() );
  return result;
}

template< std::integral IntT >
[[nodiscard]] constexpr std::array< std::byte, sizeof( IntT ) > RawData_toRaw(
  const IntT value,
  const boost::endian::order rawOrder )
{
  std::array< std::byte, sizeof( IntT ) > result;
  [[maybe_unused]] auto remaining{ RawData_setInt< IntT >( result, value, rawOrder ) };
  assert( remaining.empty() );
  return result;
}

[[nodiscard]] constexpr std::tuple< ConstRawDataSpan, std::string_view > RawData_getString(
  ConstRawDataSpan raw,
  const std::size_t stringLength )
{
  // Check minimum size
  if ( raw.size() < stringLength )
  {
    BOOST_THROW_EXCEPTION( std::out_of_range( "RawData_getString: not enough data" ) ) ;
  }

  return { raw.subspan( stringLength ), RawData_asString( raw, stringLength ) };
}

constexpr RawDataSpan RawData_setString( RawDataSpan raw, std::string_view string )
{
  // Check minimum size
  if ( raw.size() < string.size() )
  {
    BOOST_THROW_EXCEPTION( std::out_of_range( "RawData_setString: not enough data" ) ) ;
  }

  std::ranges::copy( RawData_asRaw( string ), raw.begin() );

  return raw.subspan( string.size() );
}

[[nodiscard]] inline ConstRawDataSpan RawData_asRaw( std::string_view string )
{
  return ConstRawDataSpan{ reinterpret_cast< std::byte const * >( string.data() ), string.size() };
}

constexpr std::string_view RawData_asString( ConstRawDataSpan raw )
{
  return RawData_asString( raw, raw.size() );
}

constexpr std::string_view RawData_asString( ConstRawDataSpan raw, std::size_t stringLength )
{
  // Check minimum size
  if ( raw.size() < stringLength )
  {
    BOOST_THROW_EXCEPTION( std::out_of_range( "RawData_toString: not enough data" ) ) ;
  }

  return std::string_view{ reinterpret_cast< char const * >( raw.data() ), stringLength };
}

template< typename T >
constexpr ConstRawDataSpan RawData_asRawData( std::span< const T > data ) noexcept
{
  return std::as_bytes( data );
}

template< typename T >
constexpr ConstRawDataSpan RawData_asRawData( const T &data ) noexcept( noexcept( std::data( data ) ) )
{
  return RawData_asRawData( std::span{ std::data( data ), std::size( data ) } );
}

constexpr std::byte operator ""_b( const unsigned long long int value )
{
  return std::byte{ Helper::safeCast< uint8_t >( value ) };
}

constexpr std::byte operator ""_b( const char value ) noexcept
{
  return static_cast< std::byte >( value );
}

}
