// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::MediaSetValidator.
 **/

#ifndef ARINC_665_UTILS_ARINC665VALIDATOR_HPP
#define ARINC_665_UTILS_ARINC665VALIDATOR_HPP

#include <arinc_665/utils/Utils.hpp>

#include <helper/RawData.hpp>

#include <filesystem>
#include <functional>
#include <string_view>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Validator.
 *
 * Validates the integrity and consistency of a ARINC 665 Media Set.
 **/
class ARINC_665_EXPORT MediaSetValidator
{
  public:
    /**
     * @brief Handler, which is called to read a file form a medium.
     *
     * This handler is also used to read files, which are not represented by Arinc665::Media classes.
     * Therefore, a basic representation is used.
     *
     * @param[in] mediumNumber
     *   Medium Number
     * @param[in] path
     *   Relative Path on Medium.
     *
     * @return File Data (Read as binary).
     **/
    using ReadFileHandler = std::function< Helper::RawData( uint8_t mediumNumber, const std::filesystem::path &path ) >;

    //! Handler which is called for Validation Information.
    using ValidatorInformationHandler = std::function< void( std::string_view information ) >;

    /**
     * @brief Creates the ARINC 665 %Media Set Validator Instance.
     *
     * @return ARINC 665 Validator Instance
     **/
    [[nodiscard]] static MediaSetValidatorPtr create();

    //! Destructor
    virtual ~MediaSetValidator() = default;

    /**
     * @name Configuration Methods.
     * @{
     **/

    /**
     * @brief Sets the Read File Handler.
     *
     * @param[in] readFileHandler
     *   Handler for reading files.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetValidator& readFileHandler( ReadFileHandler readFileHandler ) = 0;

    /**
     * @brief Sets the Validator Information Handler.
     *
     * @param[in] informationHandler
     *   Handler for validation information.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetValidator& informationHandler( ValidatorInformationHandler informationHandler ) = 0;

    /** @} **/

    /**
     * @brief Executes the ARINC 665 %Media Set Validator.
     *
     * All parameters must have been set previously.
     *
     * @return If the validation was successful.
     *
     * @throw Arinc665Exception
     **/
    [[nodiscard]] virtual bool operator()() = 0;
};

}

#endif
