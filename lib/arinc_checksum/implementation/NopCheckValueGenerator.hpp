// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class ArincChecksum::NopCheckValueGenerator.
 **/

#ifndef ARINC_645_IMPLEMENTATION_NOPCHECKVALUEGENERATOR_HPP
#define ARINC_645_IMPLEMENTATION_NOPCHECKVALUEGENERATOR_HPP

#include <arinc_checksum/Arinc645.hpp>
#include <arinc_checksum/CheckValueGenerator.hpp>

namespace ArincChecksum {

/**
 * @brief NOP Check Value Generator.
 *
 * This Check Value Generator does not calculate and generate any check value.
 * The data processing operations are _No Ops_.
 * @ref CheckValue::NoCheckValue is always returned.
 **/
class ARINC_645_EXPORT NopCheckValueGenerator final : public CheckValueGenerator
{
  public:
    //! Constructs CRC Check Value Generator
    NopCheckValueGenerator() = default;

    /**
     * @copydoc CheckValueGenerator::reset
     *
     * Implemented as No-Operation.
     **/
    void reset() override;

    /**
     * @copydoc CheckValueGenerator::process
     *
     * Implemented as No-Operation.
     **/
    void process( ArincSupport::ConstRawDataSpan data ) override;

    /**
     * @copydoc CheckValueGenerator::checkValue()
     *
     * @retval CheckValue::NoCheckValue always.
     **/
    [[nodiscard]] CheckValue checkValue() override;
};

}

#endif
