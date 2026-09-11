// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::MediaSetDecompiler.
 **/

#ifndef ARINC_665_UTILS_MEDIASETDECOMPILER_HPP
#define ARINC_665_UTILS_MEDIASETDECOMPILER_HPP

#include <arinc_665/utils/Utils.hpp>

#include <arinc_665/media/Media.hpp>

#include <helper/RawData.hpp>

#include <filesystem>
#include <functional>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Decompiler
 *
 * Decompiles the %Media Set given by the provided properties.
 *
 * @par Check File Integrity
 * When the *check file integrity* flag is set to `true` the checksum and check values of all media set member files and
 * the load checksum and load check values of all loads are verified.
 * The file checksum of ARINC 665 files (list of files, list of loads, list of batches, load headers, and batch files)
 * are always verified.
 **/
class ARINC_665_EXPORT MediaSetDecompiler
{
  public:
    /**
     * @brief Handler, which is called to obtain the file size.
     *
     * This handler is also used to access files, which are not represented by Arinc665::Media classes
     * (i.e. List of Files, List of Loads, and List of Batches).
     *
     * This Handler shall throw, when the file does not exist.
     *
     * @param[in] mediumNumber
     *   Medium Number
     * @param[in] path
     *   Relative Path on Medium.
     *
     * @return File size in Bytes.
     **/
    using FileSizeHandler =
      std::function< size_t( const MediumNumber &mediumNumber, const std::filesystem::path &path ) >;

    /**
     * @brief Handler, which is called to read a file from a medium.
     *
     * This handler is also used to read files, which are not represented by Arinc665::Media classes.
     *
     * This Handler shall throw when the file does not exist.
     *
     * @param[in] mediumNumber
     *   Medium Number
     * @param[in] path
     *   Relative Path on Medium.
     *
     * @return File Data (Read as binary).
     **/
    using ReadFileHandler =
      std::function< Helper::RawData( const MediumNumber &mediumNumber, const std::filesystem::path &path ) >;

    /**
     * @brief Callback for progress indication.
     *
     * @param[in] partNumber
     *   Media Set Part Number
     * @param[in] medium
     *   A @p std::pair of current medium and number of media.
     **/
    using ProgressHandler =
      std::function< void( std::string_view partNumber, std::pair< MediumNumber, MediumNumber > medium ) >;

    /**
     * @brief Creates the ARINC 665 %Media Set Decompiler Instance.
     *
     * @return ARINC 665 %Media Set Decompiler Instance
     **/
    [[nodiscard]] static MediaSetDecompilerPtr create();

    //! Destructor
    virtual ~MediaSetDecompiler() = default;

    /**
     * @name Configuration Methods.
     * @{
     **/

    /**
     * @brief Sets the %File Size Handler.
     *
     * @param[in] fileSizeHandler
     *   Handler which is called to obtain the requested file size the medium.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetDecompiler& fileSizeHandler( FileSizeHandler fileSizeHandler ) = 0;

    /**
     * @brief Sets the Read File Handler.
     *
     * @param[in] readFileHandler
     *   Handler which is called to get the requested file from the medium.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetDecompiler& readFileHandler( ReadFileHandler readFileHandler ) = 0;

    /**
     * @brief Sets the Progress Handler.
     *
     * @param[in] progressHandler
     *   Progress Handler called during operation.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetDecompiler& progressHandler( ProgressHandler progressHandler ) = 0;

    /**
     * @brief Sets the Check File Integrity Flag.
     *
     * @param[in] checkFileIntegrity
     *   If set to true, additional file integrity steps are performed.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetDecompiler& checkFileIntegrity( bool checkFileIntegrity ) noexcept = 0;

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
