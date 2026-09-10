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
 *
 * This module provides a collection of utility routines and type definitions for handling and converting raw byte data.
 * Its content is designed to facilitate processing raw data, with a focus on type conversion, endianness handling, and
 * efficient string manipulations.
 **/

#ifndef HELPER_RAWDATA_HPP
#define HELPER_RAWDATA_HPP

#include <arinc_support/Support.hpp>

#include <boost/endian.hpp>

#include <array>
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace ArincSupport {

/**
 * @name Raw Data Handling and Conversion
 *
 * Conversion of raw data types (@p std::byte) to usable types including byte order handling.
 *
 * @sa https://en.wikipedia.org/wiki/Endianness
 *
 * @{
 **/

//! Raw Data Representation
using RawData = std::vector< std::byte >;

//! Raw Data View
using RawDataSpan =  std::span< std::byte >;

//! Const Raw Data View
using ConstRawDataSpan =  std::span< const std::byte >;

/**
 * @brief Extracts an integer of a specified type from a raw data span and returns the remaining data.
 *
 * The extracted integer is converted to the native byte order if @p RawOrder differs from the system's native order.
 *
 * @tparam IntT
 *   The type of the integer to extract. Must satisfy the @p std::integral concept.
 * @tparam RawOrder
 *   The byte order of the raw data.
 *   Defaults to big-endian (@p boost::endian::order::big).
 *
 * @param[in] raw
 *   A constant span of raw data (`ConstRawDataSpan`) from which to extract the integer.
 *
 * @return A tuple containing the remaining span of raw data and the extracted integer value.
 *
 * @throws std::out_of_range
 *   if the size of the input span is less than the size of the integer type `IntT`.
 **/
template< std::integral IntT, boost::endian::order RawOrder = boost::endian::order::big >
[[nodiscard]] constexpr std::tuple< ConstRawDataSpan, IntT > RawData_getInt( ConstRawDataSpan raw );

/**
 * @brief Extracts an integer of a specified type from a raw data span and returns the remaining data.
 *
 * The extracted integer is converted to the native byte order if @p rawOrder differs from the system's native order.
 *
 * @tparam IntT
 *   The type of the integer to extract. Must satisfy the @p std::integral concept.
 *
 * @param[in] raw
 *   A constant span of raw data (`ConstRawDataSpan`) from which to extract the integer.
 * @param[in] rawOrder
 *   The byte order of the raw data.
 *
 * @return A tuple containing the remaining span of raw data and the extracted integer value.
 *
 * @throws std::out_of_range
 *   if the size of the input span is less than the size of the integer type `IntT`.
 **/
template< std::integral IntT >
[[nodiscard]] constexpr std::tuple< ConstRawDataSpan, IntT > RawData_getInt(
  ConstRawDataSpan raw,
  boost::endian::order rawOrder );

/**
 * @brief Writes an integral value into the given span of raw data, optionally performing byte-order conversion to match
 *   the specified endianness.
 *
 * This function writes an integral value to the specified raw data span, ensuring that the value is arranged in the
 * specified byte order.
 * If the native byte order differs from the specified @p RawOrder, the function performs the necessary conversion.
 * The function updates the provided span in place and returns a new span that starts after the written value, allowing
 * for chained calls or further processing.
 *
 * @tparam IntT
 *   The integral type of the value to be written.
 * @tparam RawOrder
 *   The desired byte order (endianness) for writing the value.
 *   Defaults to @p boost::endian::order::big.
 *
 * @param[in,out] raw
 *   The span of raw data in which the value will be written.
 *   Must have a size greater than or equal to @p sizeof(IntT).
 * @param[in] value
 *   The integral value to be written to the raw data span.
 *
 * @return A new @p RawDataSpan object that refers to the portion of the original span remaining after the value has
 *   been written.
 *
 * @throws std::out_of_range
 *   If the provided span does not have enough space to store the value.
 **/
template< std::integral IntT, boost::endian::order RawOrder = boost::endian::order::big >
constexpr RawDataSpan RawData_setInt( RawDataSpan raw, IntT value );

/**
 * @brief Writes an integral value into the given span of raw data, optionally performing byte-order conversion to match
 *   the specified endianness.
 *
 * This function writes an integral value to the specified raw data span, ensuring that the value is arranged in the
 * specified byte order.
 * If the native byte order differs from the specified @p rawOrder, the function performs the necessary conversion.
 * The function updates the provided span in place and returns a new span that starts after the written value, allowing
 * for chained calls or further processing.
 *
 * @tparam IntT
 *   The integral type of the value to be written.
 *
 * @param[in,out] raw
 *   The span of raw data in which the value will be written.
 *   Must have a size greater than or equal to @p sizeof(IntT).
 * @param[in] value
 *   The integral value to be written to the raw data span.
 * @param[in] rawOrder
 *   The desired byte order (endianness) for writing the value.
 *
 * @return A new @p RawDataSpan object that refers to the portion of the original span remaining after the value has
 *   been written.
 *
 * @throws std::out_of_range
 *   If the provided span does not have enough space to store the value.
 **/
template< std::integral IntT >
constexpr RawDataSpan RawData_setInt( RawDataSpan raw, IntT value, boost::endian::order rawOrder );

/**
 * @brief Converts the given integral value into a byte array using the specified byte order.
 *
 * @tparam IntT
 *   The integral type of the value to be converted.
 * @tparam RawOrder
 *   The byte order to be used during conversion.
 *   Defaults to big-endian.
 *
 * @param[in] value
 *   The integral value to be converted to a byte array.
 *
 * @return A @p std::array containing the byte representation of the input value.
 *   The size of the array is equal to the size of the integral type.
 *
 * @sa @ref RawData_setInt(RawDataSpan,IntT)
 * @sa @ref RawData_asRaw(std::string_view)
 **/
template< std::integral IntT, boost::endian::order RawOrder = boost::endian::order::big >
[[nodiscard]] constexpr std::array< std::byte, sizeof( IntT ) > RawData_toRaw( IntT value );

/**
 * @brief Converts the given integral value into a byte array using the specified byte order.
 *
 * @tparam IntT
 *   The integral type of the value to be converted.
 *
 * @param[in] value
 *   The integral value to be converted to a byte array.
 * @param rawOrder
 *   The byte order to be used during conversion.
 *
 * @return A @p std::array containing the byte representation of the input value.
 *   The size of the array is equal to the size of the integral type.
 *
 * @sa @ref RawData_setInt(RawDataSpan,IntT)
 * @sa @ref RawData_asRaw(std::string_view)
 **/
template< std::integral IntT >
[[nodiscard]] constexpr std::array< std::byte, sizeof( IntT ) > RawData_toRaw(
  IntT value,
  boost::endian::order rawOrder );

/**
 * @brief Extracts a string of a specified length from the beginning of a given raw data span.
 *
 * @param[in] raw
 *   The input raw data span from which the string will be extracted.
 * @param[in] stringLength
 *   The length of the string to extract from the raw data span.
 *
 * @return A tuple consisting of:
 *   - The remaining raw data span (after removing the extracted string).
 *   - A string_view representing the extracted string data.
 *
 * @throws std::out_of_range
 *   If the size of the raw data span is less than the specified string length.
 *
 * @note The function ensures no-copy access to the raw data as a string using std::string_view.
 *
 * @sa @ref RawData_setString(RawDataSpan,std::string_view)
 * @sa @ref RawData_asRaw(std::string_view)
 * @sa @ref RawData_asString(ConstRawDataSpan)
 * @sa @ref RawData_asString(ConstRawDataSpan,std::size_t)
 **/
[[nodiscard]] inline std::tuple< ConstRawDataSpan, std::string_view > RawData_getString(
  ConstRawDataSpan raw,
  std::size_t stringLength );

/**
 * @brief Copies the content of a string into the given raw data span.
 *
 * This function copies the content of the specified string into the raw data span, starting from the beginning of the
 * span.
 * If the size of the raw data span is smaller than the size of the string, an exception is thrown.
 *
 * @param[in,out] raw
 *   The raw data span where the string content will be copied.
 *   It must have at least the size of the string being copied.
 * @param[in] string
 *   The string whose content will be copied into the raw data span.
 *
 * @return A new RawDataSpan pointing to the remaining portion of the original raw data span, after the content of the
 *   string has been copied.
 *
 * @throws std::out_of_range
 *   If the provided raw data span is smaller than the string.
 *
 * @sa @ref RawData_getString(ConstRawDataSpan,std::size_t)
 * @sa @ref RawData_asRaw(std::string_view)
 * @sa @ref RawData_asString(ConstRawDataSpan)
 * @sa @ref RawData_asString(ConstRawDataSpan,std::size_t)
 **/
inline RawDataSpan RawData_setString( RawDataSpan raw, std::string_view string );

/**
 * @brief Converts the provided string view into a constant raw data span.
 *
 * @param[in] string
 *   The string view to be converted.
 *
 * @return A constant raw data span representing the data and size of the input string.
 *
 * @note
 * The returned span holds a view of the string data; ensure the string's lifetime outlives the usage of the span to
 * avoid dangling references.
 *
 * @sa @ref RawData_getString(ConstRawDataSpan,std::size_t)
 * @sa @ref RawData_setString(RawDataSpan,std::string_view)
 * @sa @ref RawData_asString(ConstRawDataSpan)
 * @sa @ref RawData_asString(ConstRawDataSpan,std::size_t)
 * @sa @ref RawData_toRaw(IntT)
 **/
 [[nodiscard]] inline ConstRawDataSpan RawData_asRaw( std::string_view string );

/**
 * @brief Converts raw byte data into a string view.
 *
 * This function creates a @p std::string_view from the provided constant raw data span by interpreting the byte data as
 * a character sequence.
 * The resulting string view will represent the contents of the raw byte span.
 *
 * @param[in] raw
 *   A span of constant bytes representing the raw data to be converted.
 *
 * @return A @p std::string_view representing the raw data as a string.
 *
 * @sa @ref RawData_getString(ConstRawDataSpan,std::size_t)
 * @sa @ref RawData_setString(RawDataSpan,std::string_view)
 * @sa @ref RawData_asString(ConstRawDataSpan,std::size_t)
 * @sa @ref RawData_asRaw(std::string_view)
 **/
[[nodiscard]] inline std::string_view RawData_asString( ConstRawDataSpan raw );

/**
 * @brief Converts a span of raw byte data into a string view.
 *
 * This function interprets the provided raw data as a sequence of characters and returns a string view of the specified
 * length.
 * The caller must ensure that the raw data contains at least @p stringLength bytes.
 *
 * @param[in] raw
 *   The span of raw byte data to be converted.
 * @param[in] stringLength
 *   The number of characters to include in the resulting string view.
 *
 * @return A string view representing the specified portion of the raw data.
 *
 * @throws std::out_of_range
 *   If @p raw has fewer bytes than @p stringLength.
 *
 * @sa @ref RawData_getString(ConstRawDataSpan,std::size_t)
 * @sa @ref RawData_setString(RawDataSpan,std::string_view)
 * @sa @ref RawData_asString(ConstRawDataSpan)
 * @sa @ref RawData_asRaw(std::string_view)
 **/
[[nodiscard]] inline std::string_view RawData_asString( ConstRawDataSpan raw, std::size_t stringLength );

/**
 * Converts a @p std::span of constant objects of type @p T into a @p std::span of constant raw byte data.
 *
 * @tparam T
 *   The type of elements contained in the input span.
 *
 * @param[in] data
 *   A span of objects of type T to be converted.
 *
 * @return A span of constant raw byte data representing the input span.
 **/
template< typename T >
constexpr ConstRawDataSpan RawData_asRawData( std::span< const T > data ) noexcept;

/**
 * Converts the given data of type @p T into a @p ConstRawDataSpan representation.
 *
 * This function enables the transformation of a structured data type into a view of the underlying raw data as a span
 * of constant bytes.
 * The returned @p ConstRawDataSpan allows access to the raw memory of the provided data in a read-only manner.
 *
 * @tparam T
 *   The type of the input data.
 *
 * @param[in] data
 *   The input data to be converted into a raw data span.
 *
 * @return A @p ConstRawDataSpan representing the raw memory of the provided input data.
 **/
template< typename T >
constexpr ConstRawDataSpan RawData_asRawData( const T &data ) noexcept( noexcept( std::data( data ) ) );

/**
 * @brief User-defined literal operator for creating a @p std::byte instance from an _unsigned long long integer_.
 *
 * The provided value is safely cast to a @p uint8_t.
 *
 * @param[in] value
 *   The _unsigned long long integer_ to be converted into a @p std::byte instance.
 *
 * @return A constexpr std::byte instance representing the provided value.
 *
 * @throw std::range_error
 *   if the value is out of the representable range of @p uint8_t.
 **/
constexpr std::byte operator ""_b( unsigned long long int value );

/**
 * @p User-defined literal for creating std::byte objects from char literals.
 *
 * This operator allows the direct construction of @p std::byte from a single character literal, where the character's
 * value is cast into a std::byte.
 * It simplifies the creation of byte values by eliminating the need for explicit casting.
 *
 * @param[in] value
 *   The character literal to be converted into a @p std::byte.
 *
 * @return A std::byte object representing the input character's value.
 **/
constexpr std::byte operator ""_b( char value ) noexcept;

/** @} **/

}

#include <arinc_support/RawData.ipp>

#endif
