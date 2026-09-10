// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla PublicLicense, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ Definition of Classes ArincChecksum::Arinc645Exception.
 **/

#ifndef ARINC_645_ARINC645EXCEPTION_HPP
#define ARINC_645_ARINC645EXCEPTION_HPP

#include <arinc_checksum/Arinc645.hpp>

#include <boost/exception/exception.hpp>

#include <exception>

namespace ArincChecksum {

/**
 * @brief ARINC 645 Exception.
 **/
class ARINC_645_EXPORT Arinc645Exception : public std::exception, public boost::exception
{
  public:
    /**
     * @brief Returns an Exception Description.
     *
     * @return Exception Description.
     **/
    char const * what() const noexcept override
    {
      return "ARINC 645 Exception.";
    }
};

}

#endif
