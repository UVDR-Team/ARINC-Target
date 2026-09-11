// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::TftpOptionsConfiguration.
 **/

#ifndef TFTP_TFTPOOPTIONSCONFIGURATION_HPP
#define TFTP_TFTPOOPTIONSCONFIGURATION_HPP

#include <tftp/Tftp.hpp>

#include <boost/optional.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#ifndef ARINC_615A_NO_PROGRAM_OPTIONS
#include <boost/program_options/options_description.hpp>
#endif

#include <chrono>
#include <optional>

namespace Tftp {

class TFTP_EXPORT TftpOptionsConfiguration
{
  public:
    TftpOptionsConfiguration() noexcept = default;

    explicit TftpOptionsConfiguration( const boost::property_tree::ptree &properties );

    void fromProperties( const boost::property_tree::ptree &properties );

    [[nodiscard]] boost::property_tree::ptree toProperties( bool full = false ) const;

#ifndef ARINC_615A_NO_PROGRAM_OPTIONS
    [[nodiscard]] boost::program_options::options_description options();
#endif

    bool handleTransferSizeOption{ false };

    boost::optional< uint16_t > blockSizeOption;

    boost::optional< std::chrono::seconds > timeoutOption;
};

}

#endif
