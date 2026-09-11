// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::LoadDecompiler.
 **/

#ifndef ARINC_665_UTILS_LOADDECOMPILER_HPP
#define ARINC_665_UTILS_LOADDECOMPILER_HPP

#include <arinc_665/utils/Utils.hpp>

#include <helper/RawData.hpp>

#include <filesystem>
#include <functional>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Load Decompiler
 *
 * Decompiles and analyses the load-header file of the %Load given by the provided properties.
 * All data and support files referenced by the %Load are checked.
 *
 * Finally, a pseudo-%Media Set is created, and the %Load together with all dependent data and
 * support files are added to this media set.
 * The media set part number is the part number of the %Load.
 **/
class ARINC_665_EXPORT LoadDecompiler
{
  public:
    /**
     * @brief Handler which is called to obtain the relative Path of a File.
     *
     * This handler is used to resolve file paths within the context of the %Load decompilation process.
     * It is responsible for mapping filenames to their corresponding relative paths within the load header base
     * directory.
     *
     * @param[in] filename
     *   Filename of the file for which the relative path is requested.
     *
     * @return Relative path of the file within the load header base directory.
     **/
    using FilePathHandler = std::function< std::filesystem::path( std::string_view filename ) >;

    /**
     * @brief Handler, which is called to obtain the file size.
     *
     * This handler is also used to access files, which are not represented by Arinc665::Media classes
     * (i.e. List of Files, List of Loads, and List of Batches).
     *
     * This Handler shall throw when the file does not exist.
     *
     * @param[in] filename
     *   Filename of File.
     *
     * @return File size in Bytes.
     **/
    using FileSizeHandler = std::function< size_t( const std::filesystem::path &path ) >;

    /**
     * @brief Handler, which is called to read a file.
     *
     * This Handler shall throw when the file does not exist.
     *
     * @param[in] path
     *   Relative Path to the load header base directory.
     *
     * @return File Data (Read as binary).
     **/
    using ReadFileHandler = std::function< Helper::RawData( const std::filesystem::path &path ) >;

    /**
     * @brief Creates the ARINC 665 %Media Set Decompiler Instance.
     *
     * @return ARINC 665 %Media Set Decompiler Instance
     **/
    [[nodiscard]] static LoadDecompilerPtr create();

    //! Destructor
    virtual ~LoadDecompiler() = default;

    /**
     * @name Configuration Methods.
     * @{
     **/

    /**
     * @brief Sets the File Path Handler.
     *
     * Manages and processes file path operations based on the provided configurations.
     *
     * @param[in] handler
     *   Handler which is called to get the file path.
     *
     * @return @p *this for chaining.
     **/
    virtual LoadDecompiler& filePathHandler( FilePathHandler handler ) = 0;

    /**
     * @brief Sets the %File Size Handler.
     *
     * @param[in] handler
     *   Handler which is called to get the requested file size the medium.
     *
     * @return @p *this for chaining.
     **/
    virtual LoadDecompiler& fileSizeHandler( FileSizeHandler handler ) = 0;

    /**
     * @brief Sets the Read File Handler.
     *
     * @param[in] handler
     *   Handler which is called to get the requested file from the medium.
     *
     * @return @p *this for chaining.
     **/
    virtual LoadDecompiler& readFileHandler( ReadFileHandler handler ) = 0;

    /**
     * @brief Sets the Check File Integrity Flag.
     *
     * @param[in] checkFileIntegrity
     *   If set to true, additional file integrity steps are performed.
     *
     * @return @p *this for chaining.
     **/
    virtual LoadDecompiler& checkFileIntegrity( bool checkFileIntegrity ) noexcept = 0;

    /**
     * @brief Sets the Load Filename.
     *
     * @param[in] filename
     *   Filename of Load.
     *
     * @return @p *this for chaining.
     **/
    virtual LoadDecompiler& loadFilename( std::string_view filename ) = 0;

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
