// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Helper::SeverityLevelDescription.
 **/

#include "SeverityLevelDescription.hpp"

#include <boost/exception/exception.hpp>

#ifdef ARINC_615A_NO_PROGRAM_OPTIONS
#include <stdexcept>
#else
#include <boost/program_options.hpp>
#endif

#include <boost/make_shared.hpp>

#include <helper/Format.hpp>

namespace Helper {

SeverityLevelDescription::SeverityLevelDescription() :
  Description{
    { "trace",    spdlog::level::level_enum::trace },
    { "debug",    spdlog::level::level_enum::debug },
    { "info",     spdlog::level::level_enum::info },
    { "warn",     spdlog::level::level_enum::warn },
    { "err",      spdlog::level::level_enum::err },
    { "critical", spdlog::level::level_enum::critical },
    { "off",      spdlog::level::level_enum::off }
  }
{
}

std::string SeverityLevelDescription::allLevels( std::string_view prefix ) const
{
  std::string allLevels{ prefix };

  for ( const auto &description : descriptions() )
  {
    allLevels += ARINC_FORMAT_NAMESPACE::format( "\n  - {}", description.name );
  }

  return allLevels;
}

}

namespace spdlog::level {

std::istream& operator>>( std::istream &stream, level_enum &severity )
{
  std::string severityLevelStr;
  stream >> severityLevelStr;

  const auto optionalSeverity{ Helper::SeverityLevelDescription::instance().enumeration( severityLevelStr ) };

  if ( !optionalSeverity )
  {
#ifdef ARINC_615A_NO_PROGRAM_OPTIONS
    throw std::invalid_argument{ "Invalid severity level: " + severityLevelStr };
#else
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value( severityLevelStr ) );
#endif
  }

  severity = *optionalSeverity;

  return stream;
}

}
