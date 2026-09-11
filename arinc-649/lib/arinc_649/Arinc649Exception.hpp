// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla PublicLicense, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ Definition of Classes Arinc649::Arinc649Exception.
 **/

#ifndef ARINC_649_ARINC649EXCEPTION_HPP
#define ARINC_649_ARINC649EXCEPTION_HPP

#include <arinc_649/Arinc649.hpp>

#include <boost/exception/exception.hpp>

#include <exception>

namespace Arinc649 {

/**
 * @brief ARINC 649 Exception.
 **/
class ARINC_649_EXPORT Arinc649Exception : public std::exception, public boost::exception
{
  public:
    /**
     * @brief Returns an Exception Description.
     *
     * @return Exception Description.
     **/
    char const * what() const noexcept override
    {
      return "ARINC 649 Exception.";
    }
};

}

#endif
