// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Media::Directory.
 **/

#ifndef ARINC_665_MEDIA_DIRECTORY_HPP
#define ARINC_665_MEDIA_DIRECTORY_HPP

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/ContainerEntity.hpp>

#include <string>
#include <string_view>

namespace Arinc665::Media {

/**
 * @brief %Directory within a %Media Set.
 **/
class ARINC_665_EXPORT Directory final : public ContainerEntity
{
  public:
    /**
     * @brief Initialises the instance with the given data.
     *
     * An instance cannot be created directly.
     * Refer to ContainerEntity::addDirectory() for instantiation.
     *
     * @param[in] parent
     *   Parent of this directory.
     * @param[in] name
     *   Name of the directory.
     * @param[in] createKey
     *   Key to protect creation.
     *
     * @throw Arinc665::Arinc665Exception
     *   When the parameter @p parent is not valid
     **/
    Directory( const ContainerEntityPtr &parent, std::string name, const CreateKey &createKey );

    //! @copydoc ContainerEntity::mediaSet
    [[nodiscard]] ConstMediaSetPtr mediaSet() const override;

    //! @copydoc ContainerEntity::mediaSet
    [[nodiscard]] MediaSetPtr mediaSet() override;

    //! @copydoc ContainerEntity::type
    [[nodiscard]] Type type() const override;

    //! @copydoc ContainerEntity::parent() const
    [[nodiscard]] ConstContainerEntityPtr parent() const override;

    //! @copydoc ContainerEntity::parent()
    [[nodiscard]] ContainerEntityPtr parent() override;

    //! @copydoc ContainerEntity::path() const
    [[nodiscard]] std::filesystem::path path() const override;

    /**
     * @brief Returns the name of the directory.
     *
     * @return Name of the directory.
     **/
    std::string_view name() const;

    /**
     * @brief Renames the directory.
     *
     * Checks if a directory or file exists with this name.
     *
     * @param[in] name
     *   New directory name.
     **/
    void rename( std::string name );

  private:
    /**
     * @brief Updates the parent.
     *
     * @param[in] parent
     *   New parent.
     **/
    void parent( const ContainerEntityPtr &parent );

    //! Directory Name
    std::string nameV;
    //! Parent
    ContainerEntityPtr::weak_type parentV;
};

}

#endif
