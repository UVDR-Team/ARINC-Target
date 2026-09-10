// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Template Class ArincSupport::Description.
 **/

namespace ArincSupport {

inline char const *DescriptionNotFoundException::what() const noexcept
{
  return "No description found for value";
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
const SingletonT& Description< SingletonT, EnumT, ValueT, DescEntryT >::instance()
{
  static SingletonT instance;
  return instance;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
typename Description< SingletonT, EnumT, ValueT, DescEntryT >::Iterator
Description< SingletonT, EnumT, ValueT, DescEntryT >::begin() const
{
  return descriptionV.template get< ByEnum >().begin();
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
typename Description< SingletonT, EnumT, ValueT, DescEntryT >::Iterator
Description< SingletonT, EnumT, ValueT, DescEntryT >::end() const
{
  return descriptionV.template get< ByEnum >().end();
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
const typename Description< SingletonT, EnumT, ValueT, DescEntryT >::DescriptionIndex&
Description< SingletonT, EnumT, ValueT, DescEntryT >::descriptions() const
{
  return descriptionV;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
const DescEntryT& Description< SingletonT, EnumT, ValueT, DescEntryT >::description( const Enum enumeration ) const
{
  auto &enumerationIndex{ descriptionV.template get< ByEnum >() };

  auto element{ enumerationIndex.find( enumeration ) };

  if ( element == enumerationIndex.end() )
  {
    BOOST_THROW_EXCEPTION( DescriptionNotFoundException() );
  }

  return *element;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
const DescEntryT&
Description< SingletonT, EnumT, ValueT, DescEntryT >::description( const Value value ) const
{
  auto &valueIndex{ descriptionV.template get< ByValue >() };

  auto element{ valueIndex.find( value ) };

  if ( element == valueIndex.end() )
  {
    BOOST_THROW_EXCEPTION( DescriptionNotFoundException() );
  }

  return *element;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
const DescEntryT&
Description< SingletonT, EnumT, ValueT, DescEntryT >::description( std::string_view name ) const
{
  auto &nameIndex{ descriptionV.template get< ByName >() };

  auto element{ nameIndex.find( name ) };

  if ( element == nameIndex.end() )
  {
    BOOST_THROW_EXCEPTION( DescriptionNotFoundException() );
  }

  return *element;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
std::optional< typename Description<
  SingletonT,
  EnumT,
  ValueT,
  DescEntryT >::Value >
Description< SingletonT, EnumT, ValueT, DescEntryT >::value(
  const Enum enumeration ) const
{
  auto &enumerationIndex{ descriptionV.template get< ByEnum >() };

  auto element{ enumerationIndex.find( enumeration ) };

  if ( element == enumerationIndex.end() )
  {
    return {};
  }

  return element->value;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
std::optional< typename Description< SingletonT, EnumT, ValueT, DescEntryT >::Value >
Description< SingletonT, EnumT, ValueT, DescEntryT >::value( std::string_view name ) const
{
  auto &nameIndex{ descriptionV.template get< ByName >() };

  auto element{ nameIndex.find( name ) };

  if ( element == nameIndex.end() )
  {
    return {};
  }

  return element->value;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
std::optional< typename Description< SingletonT, EnumT, ValueT, DescEntryT >::Enum >
Description< SingletonT, EnumT, ValueT, DescEntryT >::enumeration( const Value value ) const
{
  auto &valueIndex{ descriptionV.template get< ByValue >() };

  auto element{ valueIndex.find( value ) };

  if ( element == valueIndex.end() )
  {
    return {};
  }

  return element->enumeration;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
std::optional< typename Description< SingletonT, EnumT, ValueT, DescEntryT >::Enum >
Description< SingletonT, EnumT, ValueT, DescEntryT >::enumeration( std::string_view name ) const
{
  auto &nameIndex{ descriptionV.template get< ByName >() };

  auto element{ nameIndex.find( name ) };

  if ( element == nameIndex.end() )
  {
    return {};
  }

  return element->enumeration;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
std::string_view Description< SingletonT, EnumT, ValueT, DescEntryT >::name( const Value value ) const
{
  auto &valueIndex{ descriptionV.template get<ByValue>() };

  auto element{ valueIndex.find( value ) };

  if ( element == valueIndex.end() )
  {
    return {};
  }

  return element->name;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
std::string_view Description< SingletonT, EnumT, ValueT, DescEntryT >::name( const Enum enumeration ) const
{
  auto &enumerationIndex{ descriptionV.template get< ByEnum >() };

  auto element{ enumerationIndex.find( enumeration ) };

  if ( element == enumerationIndex.end() )
  {
    return {};
  }

  return element->name;
}

template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT >
Description< SingletonT, EnumT, ValueT, DescEntryT >::Description( std::initializer_list< DescEntryT > descriptions ) :
  descriptionV{ std::move( descriptions ) }
{
}

}
