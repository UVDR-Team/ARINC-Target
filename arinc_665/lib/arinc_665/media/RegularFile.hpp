// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Media::RegularFile.
 **/

#ifndef ARINC_665_MEDIA_REGULARFILE_HPP
#define ARINC_665_MEDIA_REGULARFILE_HPP

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/File.hpp>

#include <string>

namespace Arinc665::Media {

/**
 * @brief Regular %File within %Media Set.
 **/
class ARINC_665_EXPORT RegularFile final : public File
{
  public:
    /**
     * @brief Creates a regular file, which is part of a media set.
     *
     * An instance cannot be created directly.
     * Refer to ContainerEntity::addRegularFile() for instantiation.
     *
     * @param[in] parent
     *   Parent of this file.
     * @param[in] name
     *   Name of the file.
     * @param[in] mediumNumber
     *   Medium Number, where the file is located on.
     * @param[in] createKey
     *   Key to protect creation.
     **/
    RegularFile(
      const ContainerEntityPtr &parent,
      std::string name,
      const OptionalMediumNumber &mediumNumber,
      const CreateKey &createKey );

    /**
     * @copydoc File::fileType
     *
     * @retval FileType::RegularFile
     *   Always
     **/
    [[nodiscard]] FileType fileType() const override;
};

}

#endif
