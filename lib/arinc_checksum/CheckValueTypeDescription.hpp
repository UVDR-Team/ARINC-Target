// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class ArincChecksum::CheckValueTypeDescription.
 **/

#ifndef ARINC_645_CHECKVALUETYPEDESCRIPTION_HPP
#define ARINC_645_CHECKVALUETYPEDESCRIPTION_HPP

#include <arinc_checksum/Arinc645.hpp>

#include <arinc_support/Description.hpp>

#include <format>
#include <iosfwd>

namespace ArincChecksum {

/**
 * @brief Description for @ref CheckValueType Enumeration.
 *
 * @sa @ref CheckValueType
 **/
class ARINC_645_EXPORT CheckValueTypeDescription final :
  public ArincSupport::Description< CheckValueTypeDescription, CheckValueType >
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
ARINC_645_EXPORT std::ostream& operator<<( std::ostream &stream, CheckValueType type );

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
ARINC_645_EXPORT std::istream& operator>>( std::istream &stream, CheckValueType &type );

}

/**
 * @brief Specialisation of @p std::formatter for @ref ArincChecksum::CheckValueType.
 **/
template<>
struct std::formatter< ArincChecksum::CheckValueType > : std::formatter< std::string_view >
{
  /**
   * @brief ArincChecksum::CheckValueType format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] type
   *   ARINC 645 Check Value Type
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to the end of output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const ArincChecksum::CheckValueType type, FmtContext &ctx ) const
  {
    return std::formatter< string_view >::format( ArincChecksum::CheckValueTypeDescription::instance().name( type ), ctx );
  }
};

#endif
