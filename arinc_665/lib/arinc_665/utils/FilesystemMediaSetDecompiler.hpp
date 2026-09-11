// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemMediaSetDecompiler.
 **/

#ifndef ARINC_665_UTILS_FILESYSTEMMEDIASETDECOMPILER_HPP
#define ARINC_665_UTILS_FILESYSTEMMEDIASETDECOMPILER_HPP

#include <arinc_665/utils/Utils.hpp>

#include <arinc_665/utils/MediaSetDecompiler.hpp>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Decompiler using local Filesystem.
 *
 * This decompiler expects that each medium is contained as directory on the local filesystem.
 * Normal filesystem access routines are used to access the media and files.
 *
 * @sa @ref MediaSetDecompiler
 **/
class ARINC_665_EXPORT FilesystemMediaSetDecompiler
{
  public:
    /**
     * @brief Creates the ARINC 665 %Media Set Decompiler Instance.
     *
     * @return ARINC 665 %Media Set Decompiler Instance
     **/
    [[nodiscard]] static FilesystemMediaSetDecompilerPtr create();

    //! Destructor
    virtual ~FilesystemMediaSetDecompiler() = default;

    /**
     * @name Configuration Methods.
     * @{
     **/

    /**
     * @brief Sets the Progress Handler.
     *
     * @param[in] progressHandler
     *   Progress Handler called during operation.
     *
     * @return @p *this for chaining.
     **/
    virtual FilesystemMediaSetDecompiler& progressHandler( MediaSetDecompiler::ProgressHandler progressHandler ) = 0;

    /**
     * @brief Sets the Check File Integrity Flag.
     *
     * @param[in] checkFileIntegrity
     *   If set to true, additional file integrity steps are performed.
     *
     * @return @p *this for chaining.
     **/
    virtual FilesystemMediaSetDecompiler& checkFileIntegrity( bool checkFileIntegrity ) noexcept = 0;

    /**
     * @brief Sets the Media Paths
     *
     * @param[in] mediaPaths
     *   Media Paths
     *
     * @return @p *this for chaining.
     **/
    virtual FilesystemMediaSetDecompiler& mediaPaths( MediaPaths mediaPaths ) = 0;

    /** @} **/

    /**
     * @brief Executes the ARINC 665 %Media Set Decompiler.
     *
     * All parameters must have been set previously.
     *
     * @return Decompiled %Media Set
     *
     * @throw Arinc665Exception
     *   When the media set cannot be decompiled.
     **/
    [[nodiscard]] virtual MediaSetDecompilerResult operator()() = 0;
};

}

#endif
