// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Template Class ArincSupport::DescriptionEntry.
 **/

#include <arinc_support/Support.hpp>

#include <utility>

namespace ArincSupport {

template< Enumeration EnumT, Value ValueT >
DescriptionEntry< EnumT, ValueT >::DescriptionEntry( std::string name, const EnumType enumeration ) :
  name{ std::move( name ) },
  enumeration{ enumeration },
  value{ static_cast< ValueT >( ArincSupport::toUnderlying( enumeration ) ) }
{
}

template< Enumeration EnumT, Value ValueT >
DescriptionEntry< EnumT, ValueT >::DescriptionEntry(
  std::string name,
  const EnumType enumeration,
  const ValueType value ) :
  name{ std::move( name ) },
  enumeration{ enumeration },
  value{ value }
{
}

template< Enumeration EnumT, Value ValueT >
bool DescriptionEntry< EnumT, ValueT >::operator==( const EnumType compareValue ) const
{
  return enumeration == compareValue;
}

template< Enumeration EnumT, Value ValueT >
bool DescriptionEntry< EnumT, ValueT >::operator==( const ValueType compareValue ) const
{
  return value == compareValue;
}

template< Enumeration EnumT, Value ValueT >
bool DescriptionEntry< EnumT, ValueT >::operator==( std::string_view compareValue ) const
{
  return ( this->name == compareValue );
}

}
