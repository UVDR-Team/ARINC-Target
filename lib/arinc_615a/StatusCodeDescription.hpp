// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::StatusCodeDescription.
 **/

#ifndef ARINC_615A_STATUSCODEDESCRIPTION_HPP
#define ARINC_615A_STATUSCODEDESCRIPTION_HPP

#include <arinc_615a/Arinc615a.hpp>

#include <arinc_support/Description.hpp>

#include <arinc_support/Format.hpp>
#include <iosfwd>

namespace Arinc615a {

/**
 * @name ARINC 615A Status Code
 * @{
 **/

/**
 * @brief ARINC 615A Status Code Description.
 *
 * @sa @ref StatusCode
 **/
class ARINC_615A_EXPORT StatusCodeDescription final : public ArincSupport::Description< StatusCodeDescription, StatusCode >
{
  public:
    //! Initialises Instance
    StatusCodeDescription();
};

/**
 * @brief Streaming Operator to Output Status Code as String.
 *
 * The Output format is:
 * `{String Representation} ({Hex Code})`
 *
 * @param[in,out] stream
 *   Output Stream.
 * @param[in] status
 *   Status Code.
 *
 * @return Stream for Chaining.
 *
 * @sa @ref StatusCodeDescription
 * @sa @ref StatusCode
 **/
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, StatusCode status );

/**
 * @brief Streaming Operator to Output Operation Acceptance Status Code as String.
 *
 * The Output format is:
 * `{String Representation} ({Hex Code})`
 *
 * @param[in,out] stream
 *   Output Stream.
 * @param[in] status
 *   Status Code.
 *
 * @return Stream for Chaining.
 *
 * @sa @ref StatusCodeDescription
 * @sa @ref OperationAcceptanceStatusCode
 **/
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, OperationAcceptanceStatusCode status );

/** @} **/

}

#endif
