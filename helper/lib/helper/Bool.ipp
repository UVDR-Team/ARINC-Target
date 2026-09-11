// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Template Class Helper::BoolDescription.
 **/

#include <limits>

namespace Helper {

template< std::unsigned_integral IntT >
BoolDescription< IntT >::BoolDescription() :
  Description< BoolDescription< IntT >, Bool, IntT >{
    { "false", Bool::False, Value{ 0U } },
    { "true",  Bool::True,  Value{ 1U } }
  }
{
}

template< std::unsigned_integral IntT >
typename BoolDescription<IntT>::Value BoolDescription< IntT >::value( const bool boolean ) const
{
  return Description< BoolDescription< IntT >, Bool, IntT >::value( static_cast< Bool >( boolean ) ).value_or( false );
}

template< std::unsigned_integral IntT >
bool BoolDescription<IntT>::boolean( const Value value ) const
{
  const auto enumValue{ Description< BoolDescription< IntT >, Bool, IntT >::enumeration( value ) };

  if ( !enumValue )
  {
    BOOST_THROW_EXCEPTION( DescriptionNotFoundException{} );
  }

  return ( enumValue == Bool::True );
}

template< std::unsigned_integral IntT >
bool BoolDescription<IntT>::boolean( std::string_view name ) const
{
  const auto enumValue{ Description< BoolDescription< IntT >, Bool, IntT >::enumeration( name ) };

  if ( !enumValue )
  {
    BOOST_THROW_EXCEPTION( DescriptionNotFoundException{} );
  }

  return ( enumValue == Bool::True );
}

template< std::unsigned_integral IntT >
std::string_view BoolDescription<IntT>::name( const bool boolean ) const
{
  return Description< BoolDescription< IntT >, Bool, IntT >::name( static_cast< Bool >( boolean ) );
}

}
