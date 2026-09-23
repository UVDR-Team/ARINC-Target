// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::MediumNumber.
 **/

#ifndef ARINC_665_MEDIUMNUMBER_HPP
#define ARINC_665_MEDIUMNUMBER_HPP

#include <arinc_665/Arinc665.hpp>

#include <cstdint>
#include <format>
#include <iosfwd>

namespace Arinc665 {

/**
 * @brief ARINC 665 Medium Number.
 *
 * This class is used to provide an ARINC 665 Medium Number @p [1..255].
 **/
class ARINC_665_EXPORT MediumNumber final
{
  public:
    /**
     * @brief Default Constructor.
     *
     * Initialises the Medium Number to @p 1.
     **/
    explicit MediumNumber() noexcept = default;

    /**
     * @brief Constructs the Medium Number
     *
     * If the @p mediumNUmber is set to `0 `, it is automatically initialised to `1`.
     *
     * @param[in] mediumNumber
     *   Medium Number
     **/
    explicit MediumNumber( uint8_t mediumNumber ) noexcept;

    /**
     * @brief Medium number as @p std::string representation.
     *
     * @return Medium Number as @p std::string.
     **/
    [[nodiscard]] std::string toString() const;

    /**
     * @brief Binary Plus Operator with @p uin8_t.
     *
     * Clamps value to range [1..255].
     *
     * @param[in] increment
     *   Increment Value.
     *
     * @return *this
     **/
    MediumNumber operator+( uint8_t increment ) const noexcept;

    /**
     * @brief Binary Plus Minus with @p uin8_t.
     *
     * Clamps value to range [1..255].
     *
     * @param[in] decrement
     *   Decrement Value.
     *
     * @return *this
     **/
    MediumNumber operator-( uint8_t decrement ) const noexcept;

    /**
     * @brief Prefix increment operator (@p ++mediumNumber).
     *
     * Clamps value to range [1..255].
     *
     * @return *this
     **/
    MediumNumber& operator++() noexcept;

    /**
     * @brief Postfix increment operator (@p mediumNumber++).
     *
     * Clamps value to range [1..255].
     *
     * @return *this before increment
     **/
    MediumNumber operator++( int ) noexcept;

    /**
     * @brief Prefix decrement operator (@p --mediumNumber).
     *
     * Clamps value to range [1..255].
     *
     * @return *this
     **/
    MediumNumber& operator--() noexcept;

    /**
     * @brief Postfix decrement operator (@p mediumNumber--).
     *
     * Clamps value to range [1..255].
     *
     * @return *this before decrement
     **/
    MediumNumber operator--( int ) noexcept;

    /**
     * @brief Explicit cast to @p uint8_t.
     *
     * @return Medium Number @p uint8_t
     **/
    explicit operator uint8_t() const noexcept;

    /**
     * @brief Explicit cast to @p std::string.
     *
     * @return Medium Number as @p std::string.
     **/
    explicit operator std::string() const;

    /**
     * @brief Assigns @p uint8_t to medium number.
     *
     * If the @p mediumNUmber is set to `0 `, it is automatically initialised to `1`.
     *
     * @param[in] mediumNumber
     *   Medium Number
     *
     * @return *this
     **/
    MediumNumber& operator =( uint8_t mediumNumber ) noexcept;

    /**
     * @brief 3-way comparison of Medium Number.
     *
     * @param[in] rhs
     *   Compares to.
     *
     * @return Comparison.
     **/
    auto operator<=>( const MediumNumber &rhs ) const noexcept = default;

  private:
    //! Medium Number
    uint8_t mediumNumberV{ 1U };
};

/**
 * @brief Medium Number @p std::ostream output operator.
 *
 * @param[in,out] stream
 *   Output stream.
 * @param[in] mediumNumber
 *   Medium number.
 *
 * @return Output stream
 *
 * @sa @ref MediumNumber
 * @sa @ref MediumNumber::toString() const
 **/
ARINC_665_EXPORT std::ostream& operator<<( std::ostream &stream, const MediumNumber &mediumNumber );

}

namespace std {

/**
 * @brief Specialisation of @p std::formatter for @ref Arinc665::MediumNumber.
 *
 * @sa @ref Arinc665::MediumNumber
 **/
template<>
struct formatter< Arinc665::MediumNumber > : std::formatter< std::string >
{
 /**
  * @brief Arinc665::MediumNumber format routine.
  *
  * @tparam FmtContext
  *   Formatting Context
  * @param[in] mediumNumber
  *   ARINC 665 Medium Number
  * @param[in,out] ctx
  *   Formatting Context
  *
  * @return Iterator to the end of output.
  **/
 template< class FmtContext >
 FmtContext::iterator format( const Arinc665::MediumNumber &mediumNumber, FmtContext &ctx ) const
 {
  return std::formatter< string >::format( mediumNumber.toString(), ctx );
 }
};

}

#endif
