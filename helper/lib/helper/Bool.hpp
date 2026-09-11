// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Template Class Helper::BoolDescription.
 **/

#ifndef HELPER_BOOL_HPP
#define HELPER_BOOL_HPP

#include <helper/Helper.hpp>
#include <helper/Description.hpp>

#include <concepts>
#include <string_view>

namespace Helper {

/**
 * @name Boolean Type and %Description.
 *
 * @sa @ref Helper::Bool
 * @sa @ref Helper::BoolDescription
 *
 * @{
 **/

//! Boolean Value
enum class Bool
{
  //! False Value
  False,
  //! True Value
  True
};

/**
 * @brief %Description of Boolean Values.
 *
 * @tparam IntT
 *   Base integer type.
 *
 * @sa @ref Helper::Bool
 **/
template< std::unsigned_integral IntT = uint32_t >
class BoolDescription final : public Description< BoolDescription< IntT >, Bool, IntT >
{
  public:
    //! Redefinition of Value Type
    using Value = typename Description<BoolDescription< IntT>, Bool, IntT>::Value;

    using typename Description<BoolDescription< IntT>, Bool, IntT>::Enum;

    using typename Description<BoolDescription< IntT>, Bool, IntT>::DescriptionEntry;

    //! Initialises %Description.
    BoolDescription();

    // make overloaded value() Methods visible
    using Description< BoolDescription< IntT >, Bool, IntT >::value;

    /**
    * @brief Returns the value for the given boolean value.
    *
    * @param[in] boolean
    *   Boolean value.
    *
    * @return Value representation.
    **/
    [[nodiscard]] Value value( bool boolean ) const;

    /**
     * @brief Returns the boolean value for the given value.
     *
     * @param[in] value
     *   Value to decode.
     *
     * @return Boolean representation
     *
     * @throw DescriptionNotFoundException
     *   If Invalid value is decoded.
     **/
    [[nodiscard]] bool boolean( Value value ) const;

    /**
     * @brief Returns the boolean value for the given name.
     *
     * @param[in] name
     *   Name.
     *
     * @return The boolean representation
     *
     * @throw DescriptionNotFoundException
     *   If Invalid value is decoded.
     **/
    [[nodiscard]] bool boolean( std::string_view name ) const;

    // make overloaded name() Methods visible
    using Description< BoolDescription< IntT >, Bool, IntT >::name;

    /**
     * @brief Search for the name of the given boolean.
     *
     * @param[in] boolean
     *   Boolean value
     *
     * @return The name for the given boolean.
     **/
    [[nodiscard]] std::string_view name( bool boolean ) const;
};

//! Bool as uint32_t Representation
using Bools = BoolDescription< uint32_t >;

//! Boolean Description with uint32_t Representation.
using BoolsUint32 = BoolDescription< uint32_t >;

/** @} **/

}

#include <helper/Bool.ipp>

#endif
