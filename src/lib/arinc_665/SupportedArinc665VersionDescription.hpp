// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::SupportedArinc665VersionDescription.
 **/

#ifndef ARINC_665_SUPPORTEDARINC665VERSIONDESCRIPTION_HPP
#define ARINC_665_SUPPORTEDARINC665VERSIONDESCRIPTION_HPP

#include <arinc_665/Arinc665.hpp>

#include <arinc_support/Description.hpp>

#include <iosfwd>

namespace Arinc665 {

/**
 * @name Supported ARINC 665 %Version Description
 *
 * @sa @ref SupportedArinc665VersionDescription
 * @sa @ref SupportedArinc665Version
 *
 * @{
 **/

/**
 * @brief Description for SupportedArinc665Version Enumeration.
 *
 * @sa @ref SupportedArinc665Version
 **/
class ARINC_665_EXPORT SupportedArinc665VersionDescription final :
  public ArincSupport::Description< SupportedArinc665VersionDescription, SupportedArinc665Version >
{
  public:
    //! Initialises Description Values
    SupportedArinc665VersionDescription();

    /**
     * @brief Generates a list of all ARINC 665 Versions.
     *
     * This function is used to generate a string of allARINC 665 versions for command line parsing description,
     *
     * @param[in] prefix
     *   Prefix string which is prepended to the list of log-levels.
     *
     * @return List of log-levels.
     **/
    std::string allValues( std::string_view prefix ) const;
};

/**
 * @brief Supported ARINC 665 %Version @p std::ostream output operator.
 *
 * @param[in,out] stream
 *   Output Stream
 * @param[in] version
 *   ARINC 665 version.
 *
 * @return @p stream for chaining.
 **/
ARINC_665_EXPORT std::ostream& operator<<( std::ostream &stream, SupportedArinc665Version version );

/**
 * @brief Supported ARINC 665 %Version @p std::istream input operator.
 *
 * @param[in,out] stream
 *   Input Stream
 * @param[out] version
 *   Decoded ARINC 665 Version
 *
 * @return @p stream for chaining.
 *
 * @sa @ref SupportedArinc665Version
 * @sa @ref SupportedArinc665VersionDescription
 **/
ARINC_665_EXPORT std::istream& operator>>( std::istream& stream, SupportedArinc665Version &version );

/** @} **/

}

#endif
