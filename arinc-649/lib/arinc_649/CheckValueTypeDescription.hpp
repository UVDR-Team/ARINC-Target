// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc649::CheckValueTypeDescription.
 **/

#ifndef ARINC_649_CHECKVALUETYPEDESCRIPTION_HPP
#define ARINC_649_CHECKVALUETYPEDESCRIPTION_HPP

#include <arinc_649/Arinc649.hpp>

#include <helper/Description.hpp>

#include <helper/Format.hpp>
#include <iosfwd>

namespace Arinc649 {

/**
 * @brief Description for @ref CheckValueType Enumeration.
 *
 * @sa @ref CheckValueType
 **/
class ARINC_649_EXPORT CheckValueTypeDescription final :
  public Helper::Description< CheckValueTypeDescription, CheckValueType >
{
  public:
    //! Initialises Description Values
    CheckValueTypeDescription();
};

/**
 * @brief Streaming Output Operator, for Check Value Type.
 *
 * Converts the check value type to string and passes it to the stream.
 *
 * @param[in,out] stream
 *   Output stream
 * @param[in] type
 *   Check Value type
 *
 * @return @p stream for chaining.
 *
 * @sa @ref CheckValueType
 * @sa @ref CheckValueTypeDescription
 **/
ARINC_649_EXPORT std::ostream& operator<<( std::ostream &stream, CheckValueType type );

/**
 * @brief Streaming Input Operator, which Parses a String as Check Value Type.
 *
 * @param[in,out] stream
 *   Input stream
 * @param[out] type
 *   Check Value type
 *
 * @return @p stream for chaining.
 *
 * @sa @ref CheckValueType
 * @sa @ref CheckValueTypeDescription
 **/
ARINC_649_EXPORT std::istream& operator>>( std::istream &stream, CheckValueType &type );

}

/**
 * @brief Specialisation of @p ARINC_FORMAT_NAMESPACE::formatter for @ref Arinc649::CheckValueType.
 **/
template<>
struct ARINC_FORMAT_NAMESPACE::formatter< Arinc649::CheckValueType > : ARINC_FORMAT_NAMESPACE::formatter< std::string_view >
{
  /**
   * @brief Arinc649::CheckValueType format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] type
   *   ARINC 649 Check Value Type
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to the end of output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const Arinc649::CheckValueType type, FmtContext &ctx ) const
  {
    return ARINC_FORMAT_NAMESPACE::formatter< std::string_view >::format( Arinc649::CheckValueTypeDescription::instance().name( type ), ctx );
  }
};

#endif
