// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::PartNumber.
 **/

#ifndef ARINC_665_PARTNUMBER_HPP
#define ARINC_665_PARTNUMBER_HPP

#include <arinc_665/Arinc665.hpp>
#include <arinc_665/Arinc665Exception.hpp>

#include <format>
#include <string>
#include <string_view>

namespace Arinc665 {

/**
 * @brief ARINC 665 Part Number.
 *
 * An ARINC 665 part number is as follows:
 *
 * `MMMCC-SSSS-SSSS`
 *
 * whereby:
 *  - `MMM` is a unique, upper-case alphanumeric identifier that is assigned to each software supplier
 *  - `CC` is a two "check characters" generated from the other characters in the part number
 *  - `SSSSSSSS` is a software supplier defined unique product identifier consisting of upper-case alphanumeric
 *    characters except for alpha characters "I", "O", "Q" and "Z".
 *
 * @sa ARINC 665-5 Section 2.1.1 "Software Load PN Format"
 **/
class ARINC_665_EXPORT PartNumber
{
  public:
    //! Length of Manufacturer Code
    static constexpr size_t ManufacturerCodeLength{ 3U };

    //! Length of Product Identifier
    static constexpr size_t ProductIdentifierLength{ 8U };

    //! Length of the Check Code when represented as string
    static constexpr size_t CheckCodeLength{ 2U };

    //! Length of an ARINC 665 Part Number
    static constexpr size_t Length{ ManufacturerCodeLength + CheckCodeLength + ProductIdentifierLength };

    /**
     * @brief Constructs an ARINC 665 part number based on manufacturer code
     * and product identifier.
     *
     * @param[in] manufacturerCode
     *   Manufacturer code
     * @param[in] productIdentifier
     *   Product identifier
     **/
    PartNumber( std::string_view manufacturerCode, std::string_view productIdentifier );

    /**
     * @brief Constructs an ARINC 665 part number based on a part number string.
     *
     * @param[in] partNumber
     *   Part number as string
     *
     * @throw Arinc665Exception
     *   If string size is invalid
     * @throw Arinc665Exception
     *   when calculated, check code differs from provided one.
     **/
    explicit PartNumber( std::string_view partNumber );

    /**
     * @name Manufacturer Code
     * @{
     **/

    /**
     * @brief Returns the manufacturer code.
     *
     * @return The manufacturer code.
     **/
    [[nodiscard]] std::string_view manufacturerCode() const;

    /**
     * @brief Sets the manufacturer code.
     *
     * @param[in] manufacturerCode
     *   The manufacturer code.
     **/
    void manufacturerCode( std::string_view manufacturerCode );

    /** @} **/

    /**
     * @name Product Identifier
     * @{
     **/

    /**
     * @brief Returns the product identifier.
     *
     * @return Product identifier.
     **/
    [[nodiscard]] std::string_view productIdentifier() const;

    /**
     * @brief Sets the product identifier.
     *
     * @param[in] productIdentifier
     *   Product identifier.
     **/
    void productIdentifier( std::string_view productIdentifier );

    /** @} **/

    /**
     * @brief Calculates and return the check code
     *
     * @return The calculated check code
     **/
    [[nodiscard]] std::string checkCode() const;

    /**
     * @brief Returns the part number as string
     *
     * @return The part number as string
     **/
    [[nodiscard]] std::string toString() const;

  private:
    /**
     * @brief Checks the Manufacturer Code.
     *
     * @param[in] manufacturerCode
     *   Manufacturer Code to check
     **/
    static void checkManufacturerCode( std::string_view manufacturerCode );

    /**
     * @brief Checks the Product Identifier.
     *
     * @param[in] productIdentifier
     *   Product Identifier to check
     *
     * @throw Arinc665Exception
     *   if length is invalid.
     **/
    static void checkProductIdentifier( std::string_view productIdentifier );

    /**
     * @brief Checks the Check Code.
     *
     * @param[in] checkCode
     *   Check Code to check
     *
     * @throw Arinc665Exception
     *   if length of checkCode is invalid.
     * @throw std::invalid_argument
     *   When invalid string is supplied
     **/
    static void checkCheckCode( std::string_view checkCode );

    //! Manufacture Code
    std::string manufacturerCodeV;
    //! Product Identifier
    std::string productIdentifierV;
};

/**
 * @brief Part Number @p std::ostream output operator.
 *
 * @param[in,out] ostream
 *   Output stream.
 * @param[in] partNumber
 *   Part number.
 *
 * @return Output stream
 *
 * @sa @ref PartNumber
 **/
ARINC_665_EXPORT std::ostream &operator<<( std::ostream &ostream, const PartNumber &partNumber );

}

namespace std {

/**
 * @brief Specialisation of @p std::formatter for @ref Arinc665::PartNumber.
 *
 * @sa @ref Arinc665::PartNumber
 **/
template<>
struct formatter< Arinc665::PartNumber > : std::formatter< std::string >
{
  /**
   * @brief Arinc665::PartNumber format routine.
   *
   * @tparam FmtContext
   *   Formatting Context
   * @param[in] partNumber
   *   ARINC 665 Part Number
   * @param[in,out] ctx
   *   Formatting Context
   *
   * @return Iterator to the end of output.
   **/
  template< class FmtContext >
  FmtContext::iterator format( const Arinc665::PartNumber &partNumber, FmtContext &ctx ) const
  {
    return std::formatter< string >::format( partNumber.toString(), ctx );
  }
};

}

#endif
