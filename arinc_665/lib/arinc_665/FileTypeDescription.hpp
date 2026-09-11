// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::FileTypeDescription.
 **/

#ifndef ARINC_665_FILETYPEDESCRIPTION_HPP
#define ARINC_665_FILETYPEDESCRIPTION_HPP

#include <arinc_665/Arinc665.hpp>

#include <helper/Description.hpp>

#include <iosfwd>

namespace Arinc665 {

/**
 * @name File Type Description
 *
 * @sa @ref FileTypeDescription
 * @sa @ref FileType
 *
 * @{
 **/

/**
 * @brief ARINC 665 %File Type Description.
 *
 * @sa @ref FileType
 **/
class ARINC_665_EXPORT FileTypeDescription final : public Helper::Description< FileTypeDescription, FileType >
{
  public:
    //! Initialises Description.
    FileTypeDescription();
};

/**
 * @brief ARINC 665 File Type @p std::ostream output operator.
 *
 * @param[in,out] stream
 *   Output stream
 * @param[in] fileType
 *   ARINC 665 file type.
 *
 * @return @p stream for chaining.
 *
 * @sa @ref FileType
 * @sa @ref FileTypeDescription
 **/
ARINC_665_EXPORT std::ostream& operator<<( std::ostream &stream, FileType fileType );

/** @} **/

}

#endif
