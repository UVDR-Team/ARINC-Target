// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc615a::Files::ProtocolFileTypeDescription.
 **/

#ifndef ARINC_615A_FILES_PROTOCOLFILETYPEDESCRIPTION_HPP
#define ARINC_615A_FILES_PROTOCOLFILETYPEDESCRIPTION_HPP

#include <arinc_615a/files/Files.hpp>

#include <arinc_support/Description.hpp>

#include <iosfwd>
#include <arinc_support/Format.hpp>

/**
 * @name ARINC 615A Protocol %File Type Description
 * @{
 **/

namespace Arinc615a::Files {

/**
 * @brief ARINC 615A Protocol %File Type Description.
 *
 * @sa @ref ProtocolFileType
 **/
class ARINC_615A_EXPORT ProtocolFileTypeDescription final :
  public ArincSupport::Description< ProtocolFileTypeDescription, ProtocolFileType >
{
  public:
    //! Initialises Instance
    ProtocolFileTypeDescription();
};

/**
 * @brief Streaming Operator to Output Protocol File Type as String.
 *
 * @param[in,out] stream
 *   Output stream
 * @param[in] protocolFileType
 *   Protocol file type
 *
 * @return @p stream for chaining.
 *
 * @sa @ref ProtocolFileType
 * @sa @ref ProtocolFileTypeDescription
 **/
ARINC_615A_EXPORT std::ostream& operator<<( std::ostream &stream, ProtocolFileType protocolFileType );

}

/** @} **/

#endif
