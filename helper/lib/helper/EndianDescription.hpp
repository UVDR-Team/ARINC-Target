// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Template Class Helper::EndianDescription.
 **/

#ifndef HELPER_ENDIANDESCRIPTION_HPP
#define HELPER_ENDIANDESCRIPTION_HPP

#include <helper/Helper.hpp>
#include <helper/Description.hpp>

#include <boost/endian.hpp>

#include <bit>

namespace Helper {

/**
 * @name Endian Description.
 *
 * @sa @ref EndianDescription
 *
 * @{
 **/

/**
 * @brief Provides a description mapping for endianness concepts between `std::endian` and `boost::endian::order`.
 *
 * This class is a specialised implementation of the `Description` template, designed to map and describe different
 * endianness representations.
 **/
class HELPER_EXPORT EndianDescription final : public Description< EndianDescription, std::endian >
{
  public:
    /**
     * @brief Converts a `boost::endian::order` value to the corresponding `std::endian` representation.
     *
     * This static method maps the provided `boost::endian::order` value to the equivalent `std::endian` enumeration.
     * It supports big-endian, little-endian, and defaults to native endianness if the input order does not match any
     * known value.
     *
     * @param[in] order
     *   The `boost::endian::order` value specifying the desired endianness.
     *
     * @return A `std::endian` value representing the equivalent endianness.
     * @retval std::endian::big
     *   If the input `order` is `boost::endian::order::big`.
     * @retval std::endian::little
     *   If the input `order` is `boost::endian::order::little`.
     * @retval std::endian::native
     *   If the input `order` does not match a standard endianness.
     **/
    [[nodiscard]] static std::endian toEndian( boost::endian::order order );

    /**
     * @brief Converts a `std::endian` value to the corresponding `boost::endian::order` representation.
     *
     * This static method maps the provided `std::endian` value to the equivalent `boost::endian::order` enumeration.
     * It supports big-endian, little-endian, and defaults to native endianness if the input value does not correspond
     * to a specific known endianness.
     *
     * @param[in] endian
     *   The `std::endian` value specifying the desired endianness.
     *
     * @return A `boost::endian::order` value representing the equivalent endianness.
     * @retval boost::endian::order::big
     *   If the input `endian` is `std::endian::big`.
     * @retval boost::endian::order::little
     *   If the input `endian` is `std::endian::little`.
     * @retval boost::endian::order::native
     *   If the input `endian` does not match a specific endianness.
     **/
    [[nodiscard]] static boost::endian::order toEndian( std::endian endian );

    //! Initialises Description.
    EndianDescription();
};

/**
 * @brief Streaming Operator to Output endian as String.
 *
 * @param[in,out] stream
 *   Output stream
 * @param[in] endian
 *   Endian
 *
 * @return @p stream for chaining.
 *
 * @sa @ref EndianDescription
 **/
HELPER_EXPORT std::ostream& operator<<( std::ostream &stream, std::endian endian );

/**
 * @brief Streaming Operator for Parsing an Endian String as std::endian Type.
 *
 * @param[in,out] stream
 *   Input stream
 * @param[out] endian
 *   Decoded endian
 *
 * @return @p stream for chaining.
 *
 * @sa @ref EndianDescription
 **/
HELPER_EXPORT std::istream& operator>>( std::istream &stream, std::endian &endian );

/** @} **/

}

#endif
