// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Media::File.
 **/

#ifndef ARINC_665_MEDIA_FILE_HPP
#define ARINC_665_MEDIA_FILE_HPP

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/Base.hpp>

#include <arinc_665/MediumNumber.hpp>

#include <arinc_649/Arinc649.hpp>

#include <filesystem>
#include <optional>

namespace Arinc665::Media {

/**
 * @brief %Media Set %File.
 *
 * This class is used as common base for:
 *  - regular files (@ref RegularFile),
 *  - load header files (@ref Load), and
 *  - batch files (@ref Batch).
 *
 * The media set *List* files:
 * - *List of files* file
 * - *List of loads* file
 * - *List of batches* file
 * have no direct representation.
 *
 * The parent stored within this class is held as weak reference.
 **/
class ARINC_665_EXPORT File : public Base
{
  public:
    //! Destructor
    ~File() override = default;

    //! @copydoc Base::mediaSet
    [[nodiscard]] ConstMediaSetPtr mediaSet() const final;

    //! @copydoc Base::mediaSet
    [[nodiscard]] MediaSetPtr mediaSet() final;

    //! @copydoc Base::type
    [[nodiscard]] Type type() const final;

    /**
     * @copydoc Base::parent() const
     *
     * @note
     * The return value of this method is always valid.
     **/
    [[nodiscard]] ConstContainerEntityPtr parent() const final;

    //! @copydoc File::parent() const
    [[nodiscard]] ContainerEntityPtr parent() final;

    //! @copydoc Base::path() const
    [[nodiscard]] std::filesystem::path path() const final;

    /**
     * @brief Returns the Name of the %File.
     *
     * @return Name of the file.
     **/
    [[nodiscard]] std::string_view name() const;

    /**
     * @brief Renames the file.
     *
     * Checks if a directory or file exist with this name in parent container.
     *
     * @param[in] name
     *   New filename.
     *
     * @throw Arinc665::Arinc665Exception
     *   When a file or directory with same name exists within parent directory.
     **/
    void rename( std::string name );

    /**
     * @name Medium Number
     *
     * Each file is placed on a medium.
     * @{
     **/

    /**
     * @brief Returns the effective medium number.
     *
     * If the medium number is not defined, the parent effective medium number is returned.
     *
     * @return effective medium number.
     */
    [[nodiscard]] MediumNumber effectiveMediumNumber() const;

    /**
     * @brief Returns the Medium Number, where the file is located on.
     *
     * @return Medium Number, where the file is located on.
     **/
    [[nodiscard]] OptionalMediumNumber mediumNumber() const;

    /**
     * @brief Updates the Medium Number of the file.
     *
     * @param[in] mediumNumber
     *   New Medium Number.
     *   If empty it is not defined on this level
     **/
    void mediumNumber( const OptionalMediumNumber &mediumNumber );

    /** @} **/

    /**
     * @brief Returns the %File Type.
     *
     * @return %File type
     **/
    [[nodiscard]] virtual FileType fileType() const = 0;

    /**
     * @name File Check Value Type
     *
     * This information is used to determine the Check Value Type used for generation of %Media Set %Files List Check
     * Value.
     * This Value override a common value set by @ref MediaSet::filesCheckValueType().
     *
     * @{
     **/

    /**
     * @brief Returns the effective %File Check Value Type.
     *
     * If the value is not set, the effective *%Media Set %File Check Value Type* is used.
     *
     * @return Effective %File Check Value Type.
     *
     * @sa MediaSet::effectiveFilesCheckValueType.
     **/
    [[nodiscard]] Arinc649::CheckValueType effectiveCheckValueType() const;

    /**
     * @brief Returns the %File Check Value Type.
     *
     * @return %File Check Value Type
     **/
    [[nodiscard]] std::optional< Arinc649::CheckValueType > checkValueType() const;

    /**
     * @brief Updates the %File Check Value Type.
     *
     * @param[in] type
     *   New %File Check Value Type.
     **/
    void checkValueType( std::optional< Arinc649::CheckValueType > type );

    /** @} **/

  protected:
    /**
     * @brief Initialises the instance with the given data.
     *
     * @param[in] parent
     *   Parent container.
     * @param[in] name
     *   Name of the element.
     * @param[in] mediumNumber
     *   Medium Number, where the file is located on.
     *
     * @throw Arinc665Exception
     *   If the @p parent is empty or the @p name is empty.
     **/
    File( const ContainerEntityPtr &parent, std::string name, const OptionalMediumNumber &mediumNumber );

    /**
     * @brief Sets the parent element.
     *
     * This operation is used to move a file to another directory.
     *
     * @attention
     * The new parent and the file must reside on the same media set.
     *
     * @param[in] parent
     *   Parent element.
     *
     * @throw Arinc665Exception
     *   If parent is invalid or does not reside on the same media set as @p this.
     **/
    void parent( const ContainerEntityPtr &parent );

  private:
    //! Parent Container
    ContainerEntityPtr::weak_type parentV;
    //! Filename
    std::string nameV;
    //! Medium Number
    OptionalMediumNumber mediumNumberV;
    //! Check Value Type
    std::optional< Arinc649::CheckValueType > checkValueTypeV;
};

}

#endif
