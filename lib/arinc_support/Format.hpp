// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC_SUPPORT_FORMAT_HPP
#define ARINC_SUPPORT_FORMAT_HPP

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ArincSupport {
namespace Detail {

template< typename T >
void streamValue( std::ostream &stream, const T &value )
{
  using ValueT = typename std::decay< T >::type;
  if constexpr ( std::is_same< ValueT, char >::value
    || std::is_same< ValueT, signed char >::value
    || std::is_same< ValueT, unsigned char >::value )
  {
    stream << static_cast< int >( value );
  }
  else
  {
    stream << value;
  }
}

template< typename T >
std::string formatValue( std::string_view specification, const T &value )
{
  std::ostringstream stream;
  std::string spec{ specification };
  if ( !spec.empty() && spec.front() == ':' )
  {
    spec.erase( spec.begin() );
  }

  bool hexadecimal{ false };
  bool uppercase{ false };
  if ( !spec.empty() && ( spec.back() == 'X' || spec.back() == 'x' ) )
  {
    hexadecimal = true;
    uppercase = spec.back() == 'X';
    spec.pop_back();
  }
  else if ( !spec.empty() && spec.back() == 'd' )
  {
    spec.pop_back();
  }

  char fill{ ' ' };
  if ( spec.size() > 1U && spec.front() == '0' )
  {
    fill = '0';
  }

  if ( !spec.empty() )
  {
    const auto width{ static_cast< int >( std::stoul( spec ) ) };
    stream << std::setfill( fill ) << std::setw( width );
    using ValueT = typename std::decay< T >::type;
    if ( fill == ' ' && !std::is_arithmetic< ValueT >::value && !std::is_enum< ValueT >::value )
    {
      stream << std::left;
    }
  }

  if ( hexadecimal )
  {
    stream << std::hex;
    if ( uppercase )
    {
      stream << std::uppercase;
    }
  }
  streamValue( stream, value );
  return stream.str();
}

inline void appendLiteral( std::string &output, std::string_view text )
{
  for ( std::size_t index = 0; index < text.size(); ++index )
  {
    if ( index + 1U < text.size() && text[ index ] == text[ index + 1U ]
      && ( text[ index ] == '{' || text[ index ] == '}' ) )
    {
      output.push_back( text[ index ] );
      ++index;
    }
    else
    {
      output.push_back( text[ index ] );
    }
  }
}

template< std::size_t Index, typename TupleT >
void appendArguments( std::string &output, std::string_view pattern, const TupleT &arguments )
{
  if constexpr ( Index == std::tuple_size< TupleT >::value )
  {
    if ( pattern.find( '{' ) != std::string_view::npos )
    {
      throw std::invalid_argument{ "Not enough ARINC format arguments" };
    }
    appendLiteral( output, pattern );
  }
  else
  {
    std::size_t fieldBegin{ std::string_view::npos };
    for ( std::size_t index = 0; index < pattern.size(); ++index )
    {
      if ( pattern[ index ] == '{' )
      {
        if ( index + 1U < pattern.size() && pattern[ index + 1U ] == '{' )
        {
          ++index;
          continue;
        }
        fieldBegin = index;
        break;
      }
    }
    if ( fieldBegin == std::string_view::npos )
    {
      throw std::invalid_argument{ "Too many ARINC format arguments" };
    }
    const auto fieldEnd{ pattern.find( '}', fieldBegin + 1U ) };
    if ( fieldEnd == std::string_view::npos )
    {
      throw std::invalid_argument{ "Unterminated ARINC format field" };
    }
    appendLiteral( output, pattern.substr( 0U, fieldBegin ) );
    output += formatValue( pattern.substr( fieldBegin + 1U, fieldEnd - fieldBegin - 1U ), std::get< Index >( arguments ) );
    appendArguments< Index + 1U >( output, pattern.substr( fieldEnd + 1U ), arguments );
  }
}

}

template< typename... Args >
[[nodiscard]] std::string format( std::string_view pattern, Args&&... args )
{
  std::string output;
  output.reserve( pattern.size() + sizeof...( Args ) * 8U );
  const auto arguments{ std::forward_as_tuple( std::forward< Args >( args )... ) };
  Detail::appendArguments< 0U >( output, pattern, arguments );
  return output;
}

template< typename OutputIterator, typename... Args >
OutputIterator formatTo( OutputIterator output, std::string_view pattern, Args&&... args )
{
  const auto text{ format( pattern, std::forward< Args >( args )... ) };
  return std::copy( text.begin(), text.end(), output );
}

}

#endif
