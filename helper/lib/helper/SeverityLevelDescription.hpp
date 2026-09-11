// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Helper::SeverityLevelDescription.
 **/

#ifndef HELPER_SEVERITYLEVELDESCRIPTION_HPP
#define HELPER_SEVERITYLEVELDESCRIPTION_HPP

#include <helper/Helper.hpp>
#include <helper/Description.hpp>

#include <spdlog/common.h>

#include <iosfwd>

#include <string>
#include <string_view>

/**
 * @name Severity Level Description.
 *
 * @sa @ref SeverityLevelDescription
 * @sa @ref spdlog::level::level_enum
 *
 * @{
 **/

namespace Helper {

/**
 * @brief  Severity Level Description.
 **/
class HELPER_EXPORT SeverityLevelDescription final :
  public Description< SeverityLevelDescription, spdlog::level::level_enum >
{
  public:
    //! Constructor
    SeverityLevelDescription();

    /**
     * @brief Generates a list of all log-levels.
     *
     * This function is used to generate a string of all log-levels for command line parsing description,
     *
     * @param[in] prefix
     *   Prefix string which is prepended to the list of log-levels.
     *
     * @return List of log-levels.
     **/
    std::string allLevels( std::string_view prefix = "The verbosity of the logging information:" ) const;
};

}

namespace spdlog::level {

/**
 * @brief Parse a string to a severity level.
 *
 * @param[in,out] stream
 *   Input stream
 * @param[out] severity
 *   The parsed severity level
 *
 * @return @p stream
 *
 * @throw boost::program_options::invalid_option_value
 *   when the given Severity Level cannot be parsed
 **/
HELPER_EXPORT std::istream& operator>>( std::istream &stream, level_enum &severity );

}

/** @} **/

#endif
