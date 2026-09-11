// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc649::CheckValueGenerator.
 **/

#ifndef ARINC_649_ARINC649_CHECKVALUEGENERATOR_HPP
#define ARINC_649_ARINC649_CHECKVALUEGENERATOR_HPP

#include <arinc_649/Arinc649.hpp>

#include <helper/RawData.hpp>

#include <filesystem>
#include <optional>

namespace Arinc649 {

/**
 * @brief ARINC 649 Check Value Generator.
 *
 * This class encapsulates various check value generators, which are defined by ARINC 649.
 *
 * @sa @ref CheckValueGenerator::create()
 **/
class ARINC_649_EXPORT CheckValueGenerator
{
  public:
    //! Destructor
    virtual ~CheckValueGenerator() = default;

    /**
     * @brief Calculate the Check Value over the given Data.
     *
     * Allocates a temporary check value generator instance and calculates the check value over the given data.
     *
     * @param[in] type
     *   Check Value Type.
     * @param[in] data
     *   Data to calculate the checksum from.
     *
     * @return Calculated Check Value
     * @retval std::nullopt
     *   When the parameter @p type is invalid.
     **/
    [[nodiscard]] static std::optional< CheckValue > checkValue( CheckValueType type, Helper::ConstRawDataSpan data );

    /**
     * @brief Calculates an ARINC 649 Check Value of a given file.
     *
     * @param[in] file
     *   Filename
     * @param[in] checkValueType
     *   Check Value Type
     *
     * @return Calculated Check Value.
     *
     * @throw Arinc649Exception
     *   Check value type is invalid.
     * @throw Arinc649Exception
     *   File does not exist.
     * @throw Arinc649Exception
     *   Read Error occurs.
     **/
    [[nodiscard]] static CheckValue checkValue( CheckValueType checkValueType, const std::filesystem::path &file );

    /**
     * @brief Creates Check Value Generator Instance.
     *
     * @param[in] type
     *   Check Value Type.
     *
     * @return Check Value Generator Instance.
     * @retval CheckValueGeneratorPtr{}
     *   When @p type is invalid.
     **/
    [[nodiscard]] static CheckValueGeneratorPtr create( CheckValueType type );

    /**
     * @brief Reset Check Value to the initial state.
     *
     * This operation shall be called before the desired generator is reused.
     *
     * @sa @ref CheckValueGenerator::process()
     * @sa @ref CheckValueGenerator::checkValue()
     **/
    virtual void reset() = 0;

    /**
     * @brief Process the given Data.
     *
     * Processes the data for check value calculation.
     * This operation can be called multiple times if the check value cannot be calculated at once, e.g. when processing
     * data block-wise.
     *
     * @param[in] data
     *   Data to be processed.
     *
     * @sa @ref CheckValueGenerator::reset()
     * @sa @ref CheckValueGenerator::checkValue()
     **/
    virtual void process( Helper::ConstRawDataSpan data ) = 0;

    /**
     * @brief Return the final Check Value.
     *
     * If necessary, this operation performs the final calculation of the check value.
     *
     * @note
     * Calling this operation multiple times on the same check value could possibly generate different chack values.
     * Before reusing the check value generator instance call @ref reset().
     *
     * @return Final Check Value
     *
     * @sa @ref CheckValueGenerator::reset()
     * @sa @ref CheckValueGenerator::process()
     **/
    [[nodiscard]] virtual CheckValue checkValue() = 0;
};

}

#endif
