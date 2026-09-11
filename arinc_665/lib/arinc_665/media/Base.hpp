// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Media::Base.
 **/

#ifndef ARINC_665_MEDIA_BASE_HPP
#define ARINC_665_MEDIA_BASE_HPP

#include <arinc_665/media/Media.hpp>

#include <filesystem>
#include <memory>

namespace Arinc665::Media {

/**
 * @brief %Media Set Item.
 *
 * %Base class for media set classes.
 * Provides common attributes.
 */
class ARINC_665_EXPORT Base : public std::enable_shared_from_this< Base >
{
  public:
    /**
     * @brief Create Key for controlled Creation
     *
     * @sa https://arne-mertz.de/2016/10/passkey-idiom/
     **/
    class CreateKey final
    {
      public:
        //! Deleted Copy Constructor
        CreateKey( CreateKey &other ) = delete;
        //! Deleted Move Constructor
        CreateKey( CreateKey &&other ) = delete;
        //! Deleted Assignment Operator
        CreateKey& operator =( CreateKey &other ) = delete;
        //! Deleted Move Assignment Operator
        CreateKey& operator =( CreateKey &&other ) = delete;

      private:
        //! Default Constructor
        CreateKey() = default;

        friend class MediaSet;
        friend class ContainerEntity;
    };

    //! Default Constructor
    Base() noexcept = default;

    //! Copy Constructor (deleted)
    Base( const Base& ) = delete;
    //! Move Constructor (deleted)
    Base( Base&& ) = delete;
    //! Copy Assignment Operator (deleted)
    Base& operator =( const Base& ) = delete;
    //! Move Assignment Operator (deleted)
    Base& operator =( Base&& ) = delete;

    //! Destructor
    virtual ~Base() noexcept = default;

    /**
     * @brief Returns the Owning %Media Set.
     *
     * @return Owning %Media Set
     **/
    [[nodiscard]] virtual ConstMediaSetPtr mediaSet() const = 0;

    //! @copydoc mediaSet() const
    [[nodiscard]] virtual MediaSetPtr mediaSet() = 0;

    /**
     * @brief Returns the Item Type.
     *
     * @return Item Type
     **/
    [[nodiscard]] virtual Type type() const = 0;

    /**
     * @name Parent
     * @{
     **/

    /**
     * @brief Returns the parent.
     *
     * @return Parent of this container.
     **/
    [[nodiscard]] virtual ConstContainerEntityPtr parent() const = 0;

    //! @copydoc parent() const
    [[nodiscard]] virtual ContainerEntityPtr parent() = 0;

    /** @} **/

    /**
     * @brief Returns the path up to the media set root.
     *
     * The path is an absolute path originating in the medium root directory.
     *
     * @return The path up to the media set root.
     **/
    [[nodiscard]] virtual std::filesystem::path path() const = 0;
};

}

#endif
