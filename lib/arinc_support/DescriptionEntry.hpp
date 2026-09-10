// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Template Class DescriptionEntry.
 **/

#ifndef HELPER_DESCRIPTIONENTRY_HPP
#define HELPER_DESCRIPTIONENTRY_HPP

#include <arinc_support/Support.hpp>

#include <concepts>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace ArincSupport {

/**
 * @brief Is Enumeration Concept.
 *
 * Requires that @p T is an enumeration type.
 *
 * @tparam T
 *   Enumeration Type
 **/
template< typename T >
concept Enumeration = std::is_enum_v< T >;

/**
 * @brief Is Value Type Concept.
 *
 * Requires that @p T is an integral type or enumeration type.
 *
 * @tparam T
 *   Enumeration Type
 **/
template< typename T >
concept Value = std::is_enum_v< T > || std::is_integral_v< T >;

/**
 * @brief Value Description extends an enumeration by an assigned value.
 *
 * @tparam EnumT
 *   Type of Enumeration in struct.
 * @tparam ValueT
 *   Type of Value in struct - defaults to enumeration underlying type.
 **/
template< Enumeration EnumT, Value ValueT = typename std::underlying_type_t< EnumT > >
class DescriptionEntry
{
  public:
    //! Enumeration Type
    using EnumType = EnumT;
    //! Value Type
    using ValueType = ValueT;

    //! Name of Enumeration
    const std::string name;
    //! Enumeration
    const EnumType enumeration;
    //! Value of Enumeration
    const ValueType value;

    /**
     * @brief Initialise Description Entry
     *
     * The value is the enumeration value cast to its value type.
     *
     * @param[in] name
     *   Name.
     * @param[in] enumeration
     *   Enumeration Value.
     **/
    DescriptionEntry( std::string name, EnumType enumeration );

    /**
     * @brief Initialise Description Entry
     *
     * @param[in] name
     *   Name.
     * @param[in] enumeration
     *   Enumeration Value.
     * @param[in] value
     *   Associated Value.
     **/
    DescriptionEntry( std::string name, EnumType enumeration, ValueType value );

    /**
     * @brief Defaulted Copy Assignment Constructor.
     *
     * @param[in] other
     *   Object to assign.
     **/
    DescriptionEntry( const DescriptionEntry &other ) = default;

    /**
     * @brief Defaulted Move Assignment Constructor.
     *
     * @param[in] other
     *   Object to assign.
     **/
    DescriptionEntry( DescriptionEntry &&other ) = default;

    // Delete Copy Assignment Operator
    DescriptionEntry& operator =( const DescriptionEntry &other ) = delete;

    // Delete Move Assignment Operator
    DescriptionEntry& operator ==( DescriptionEntry &&other ) = delete;

    /**
     * @brief Compare the Description class against the value type.
     *
     * @param[in] compareValue
     *   Value to compare.
     *
     * @return Equality of description and value.
     * @retval true
     *   CompareValue is equal to local value.
     * @retval false
     *   CompareValue is unequal to local value.
     **/
    bool operator==( EnumType compareValue ) const;

    /**
     * @brief Compare the Description class against the value type.
     *
     * @param[in] compareValue
     *   Value to compare.
     *
     * @return Equality of description and value.
     * @retval true
     *   CompareValue is equal to local value.
     * @retval false
     *   CompareValue is unequal to local value.
     **/
    bool operator==( ValueType compareValue ) const;

    /**
     * @brief Compare the Description class against the name.
     *
     * @param[in] compareValue
     *   Name to compare.
     *
     * @return Equality of description and name.
     * @retval true
     *   CompareValue is equal to local name.
     * @retval false
     *   CompareValue is unequal to local name.
     **/
    bool operator==( std::string_view compareValue ) const;
};

}

#include <arinc_support/DescriptionEntry.ipp>

#endif
