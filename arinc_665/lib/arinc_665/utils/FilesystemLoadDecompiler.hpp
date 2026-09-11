// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemLoadDecompiler.
 **/

#ifndef ARINC_665_UTILS_FILESYSTEMLOADDECOMPILER_HPP
#define ARINC_665_UTILS_FILESYSTEMLOADDECOMPILER_HPP

#include <arinc_665/utils/Utils.hpp>

#include <arinc_665/media/Media.hpp>

#include <filesystem>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Load Decompiler
 *
 * Decompiles the %Load given by the provided properties.
 * This class makes use of the @ref LoadDecompiler. For File-handling the approbate functions are
 * implemented.
 * The implementation expects that all load-related files are located relative to the base directory of the load header
 * file.
 * Also, subdirectories are checked for load-related files.
 *
 * @sa @ref LoadDecompiler
 **/
class ARINC_665_EXPORT FilesystemLoadDecompiler
{
  public:
    /**
     * @brief Creates the ARINC 665 %Media Set Decompiler Instance.
     *
     * @return ARINC 665 %Media Set Decompiler Instance
     **/
    [[nodiscard]] static FilesystemLoadDecompilerPtr create();

    //! Destructor
    virtual ~FilesystemLoadDecompiler() = default;

    /**
     * @name Configuration Methods.
     * @{
     **/

    /**
     * @brief Sets the Check File Integrity Flag.
     *
     * @param[in] checkFileIntegrity
     *   If set to true, additional file integrity steps are performed.
     *
     * @return @p *this for chaining.
     **/
    virtual FilesystemLoadDecompiler& checkFileIntegrity( bool checkFileIntegrity ) noexcept = 0;

    /**
     * @brief Sets the Load Path.
     *
     * @param[in] path
     *   Path of Load Header File.
     *
     * @return @p *this for chaining.
     **/
    virtual FilesystemLoadDecompiler& loadPath( std::filesystem::path path ) = 0;

    /** @} **/

    /**
     * @brief Executes the ARINC 665 %Load Decompiler.
     *
     * All parameters must have been set previously.
     *
     * @return Decompiled %Load Information
     *
     * @throw Arinc665Exception
     *   When the media set cannot be decompiled.
     **/
    [[nodiscard]] virtual LoadDecompilerResult operator()() = 0;
};

}

#endif
