// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Template ArincChecksum::HashCheckValueGenerator.
 **/

#ifndef ARINC_645_IMPLEMENTATION_HASHCHECKVALUEGENERATOR_HPP
#define ARINC_645_IMPLEMENTATION_HASHCHECKVALUEGENERATOR_HPP

#include <arinc_checksum/Arinc645.hpp>
#include <arinc_checksum/CheckValueGenerator.hpp>

namespace ArincChecksum {

/**
 * @brief Secure Hash Check Value Generator.
 *
 * This generator is used to create the Check Values:
 * - MD5,
 * - SHA1,
 * - SHA-256, and
 * - SHA-512
 *
 * This implementation uses the Boost::Hash2 Library.
 *
 * @tparam CVT
 *   Check Value Type to return on check value.
 * @tparam HashFunctionT
 *   The Hash Function Class to use
 **/
template< CheckValueType CVT, typename HashFunctionT >
class HashCheckValueGenerator final : public CheckValueGenerator
{
  public:
    /**
     * @brief Constructs Hash Check Value Generator
     **/
    HashCheckValueGenerator() = default;

    //! @copydoc CheckValueGenerator::reset
    void reset() override;

    //! @copydoc CheckValueGenerator::process
    void process( ArincSupport::ConstRawDataSpan data ) override;

    //! @copydoc CheckValueGenerator::checkValue()
    [[nodiscard]] CheckValue checkValue() override;

  private:
    //! Hash Function
    HashFunctionT hashFunction;
};

}

#include <arinc_checksum/implementation/HashCheckValueGenerator.ipp>

#endif
