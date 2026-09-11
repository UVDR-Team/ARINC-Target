// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Arinc649 CheckValue.
 **/

#ifndef ARINC_649_ARINC649_CHECKVALUE_HPP
#define ARINC_649_ARINC649_CHECKVALUE_HPP

#include <arinc_649/Arinc649.hpp>

#include <helper/RawData.hpp>

#include <format>
#include <iosfwd>
#include <map>
#include <optional>
#include <span>
#include <string>

namespace Arinc649 {

/**
 * @brief ARINC 649 Check Value
 *
 * A Check Value encapsulates a *Checksum Type* and its *Checksum Value*.
 * The default constructed check value represents _No Check Value_.
 **/
class ARINC_649_EXPORT CheckValue final
{
  public:
    //! No specific Check Value
    static const CheckValue NoCheckValue;

    /**
     * @brief Retrieves a mapping of CheckValueType to its corresponding size in bytes.
     *
     * This function provides a static mapping between each CheckValueType and the size (in bytes) of its associated
     * checksum or hash value.
     *
     * @return A constant reference to a map containing CheckValueType as keys and their sizes in bytes as values.
     **/
    [[nodiscard]] static const std::map< CheckValueType, uint16_t >& checkValuesSize();

    /**
     * @name CRC-16
     *
     * The CRC-16 is used within ARINC 665 files as plain representation.
     * To convert between plain @p uint16_t and @ref CheckValue representation, these operations are provided.
     *
     * @{
     **/

    /**
     * @brief Converts given Check Value to a single CRC-16.
     *
     * This operation can be used to convert a given check value to its @p uint16_t CRC-16 representation.
     * The plain CRC-16, for example, is stored within ARINC 665 files.
     *
     * @note
     * The Check Value type must be CRC-16, otherwise @p std::nullopt is returned.
     *
     * @param[in] checkValue
     *   Check Value representing an CRC-16.
     *
     * @return Converted CRC 16 (Host Byte Order)
     * @retval std::nullopt
     *   When @p checkValue is not a CRC-16 Check Value.
     **/
    [[nodiscard]] static std::optional< uint16_t > crc16( const CheckValue &checkValue );

    /**
     * @brief Converts given CRC-16 to a Check Value.
     *
     * This operation can be used to convert an @p uint16_t CRC-16 to its check value representation.
     * The plain CRC-16, for example, is stored within ARINC 665 files.
     *
     * @param[in] crc
     *   CRC-16 Value.
     *
     * @return Converted Check Value
     **/
    [[nodiscard]] static CheckValue crc16( uint16_t crc );

    /** @} **/

    //! Default Constructor
    CheckValue() = default;

    /**
     * @brief Initialises the Check Value with the given value.
     *
     * @param[in] type
     *   Check Value Type
     * @param[in] value
     *   Check Value
     **/
    CheckValue( CheckValueType type, Helper::RawData value );

    /**
     * @brief Converts a given string to Check Value.
     *
     * When @p string is not even, the check value is not converted.
     *
     * @param[in] type
     *   Expected check value type.
     * @param[in] string
     *   Check Value String.
     **/
    CheckValue( CheckValueType type, std::string_view string );

    /**
     * @brief Converts the given Check Value to String.
     *
     * The check value is converted to hexadecimal format.
     *
     * @return String value representation of @p valueV.
     **/
    [[nodiscard]] std::string toString() const;

    /**
     * @brief Formats the Check Value to @p std::string.
     *
     * This Operation can be used for formatting a Check Value for debugging purpose.
     * The format is `TYPE:VALUE`.
     *
     * @return Check Value formatted as a string.
     *
     * @sa @ref CheckValue::toString()
     **/
    [[nodiscard]] std::string format() const;

    /**
     * @brief Returns Check Value Type.
     *
     * @return Check Value Type
     **/
    [[nodiscard]] CheckValueType type() const;

    /**
     * @brief Returns the Check Value.
     *
     * @return Check Value
     **/
    [[nodiscard]] Helper::ConstRawDataSpan value() const;

    /**
     * @brief Checks Check Value for validity.
     *
     * Checks:
     * - The check value type is valid
     * - The check value length matches the check value type.
     *
     * @return If the check value is valid.
     **/
    explicit operator bool() const;

    /**
     * @brief Comparison Operator
     *
     * @param[in] other
     *   Check Value to compare to @p this
     *
     * @return Comparison information
     **/
    auto operator<=>( const CheckValue &other ) const = default;

  private:
    //! Check Value Type
    CheckValueType typeV{ CheckValueType::NotUsed };
    //! Check Value
    Helper::RawData valueV;
};

/**
 * @brief Check Value Stream Output Operator.
 *
 * Converts the Check Value into string.
 *
 * @param[in,out] stream
 *   Output Stream
 * @param checkValue
 *   Check Value
 *
 * @return @p stream for chaining
 *
 * @sa @ref CheckValue::format()
 **/
ARINC_649_EXPORT std::ostream& operator<<( std::ostream &stream, const CheckValue &checkValue );

}

/**
 * @brief Specialisation of @p std::formatter for @ref Arinc649::CheckValue
 **/
template<>
struct std::formatter< Arinc649::CheckValue > : std::formatter< std::string_view >
{
  /**
   * @brief Arinc649::CheckValue format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] checkValue
   *   ARINC 649 Check Value
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to end of output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const Arinc649::CheckValue &checkValue, FmtContext &ctx ) const
  {
    return std::formatter< string_view >::format( checkValue.format(), ctx );
  }
};

#endif
