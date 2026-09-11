// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ Definition of Class Arinc665::Arinc665Exception.
 **/

#ifndef ARINC_665_ARINC665EXCEPTION_HPP
#define ARINC_665_ARINC665EXCEPTION_HPP

#include <arinc_665/Arinc665.hpp>

#include <boost/exception/exception.hpp>

#include <exception>

namespace Arinc665 {

//! ARINC 665 Exception.
class ARINC_665_EXPORT Arinc665Exception : public std::exception, public boost::exception
{
  public:
    /**
     * @brief Returns the exception description.
     * @return The exception description.
     **/
    [[nodiscard]] const char * what() const noexcept override
    {
      return "ARINC 665 Exception";
    }
};

//! Invalid ARINC 665 %File Exception.
class ARINC_665_EXPORT InvalidArinc665File : public Arinc665Exception
{
  public:
    /**
     * @brief Returns the exception description.
     * @return The exception description.
     **/
    [[nodiscard]] const char* what() const noexcept override
    {
      return "Invalid ARINC 665 File Exception";
    }
};

}

#endif
