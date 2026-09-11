// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemMediaSetCompiler.
 **/

#ifndef ARINC_665_UTILS_FILESYSTEMMEDIASETCOMPILER_HPP
#define ARINC_665_UTILS_FILESYSTEMMEDIASETCOMPILER_HPP

#include <arinc_665/utils/Utils.hpp>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Compiler using local Filesystem.
 *
 * Normal filesystem access routines are used to create and access the media and files.
 *
 * @sa @ref MediaSetCompiler
 **/
class ARINC_665_EXPORT FilesystemMediaSetCompiler
{
  public:
    /**
     * @brief Creates the ARINC 665 %Media Set Compiler Instance.
     *
     * @return ARINC 665 %Media Set Compiler Instance
     **/
    [[nodiscard]]  static FilesystemMediaSetCompilerPtr create();

    //! Destructor
    virtual ~FilesystemMediaSetCompiler() = default;

    /**
     * @name Configuration Methods.
     * @{
     **/

    /**
     * @brief Sets the Media Set to compile.
     *
     * If the media set name is not set (is empty), the name is set to the media set part number.
     *
     * @sa FilesystemMediaSetCompiler::mediaSetName()
     *
     * @param[in] mediaSet
     *   Media Set, which shall be exported.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCompiler& mediaSet( Media::ConstMediaSetPtr mediaSet ) = 0;

    /**
     * @brief Sets the ARINC 665 Version Flag.
     *
     * @param[in] version
     *   ARINC 665 version used for exporting.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCompiler& arinc665Version( SupportedArinc665Version version ) = 0;

    /**
     * @brief Sets the Create Batch Files Flag.
     *
     * @param[in] createBatchFiles
     *   Defines if Batch Files are created by exporter or pre-existing ones are used.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCompiler& createBatchFiles( FileCreationPolicy createBatchFiles ) = 0;

    /**
     * @brief Sets the Create Load Header Files Flag.
     *
     * @param[in] createLoadHeaderFiles
     *   Defines if Load Header Files are created by exporter or pre-existing ones are used.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCompiler& createLoadHeaderFiles( FileCreationPolicy createLoadHeaderFiles ) = 0;

    /**
     * @brief Updates the base directory for source files if the path within the file mapping table is relative.
     *
     * @param[in] sourceBasePath
     *   Sources Base directory.
     *
     * @return *this for chaining.
     *
     * @sa @ref FilesystemMediaSetCompiler::filePathMapping
     **/
    virtual FilesystemMediaSetCompiler& sourceBasePath( std::filesystem::path sourceBasePath ) = 0;

    /**
     * @brief Updates the File Mapping Table.
     *
     * This file mapping table sets the paths for the Media Set Files.
     *
     * @param[in] filePathMapping
     *   File mapping table
     *
     * @return *this for chaining.
     *
     * @sa @ref FilesystemMediaSetCompiler::filePathMapping
     **/
    virtual FilesystemMediaSetCompiler& filePathMapping( FilePathMapping filePathMapping ) = 0;

    /**
     * @brief Updates the base-directory where the Media Set will be created.
     *
     * @param[in] outputBasePath
     *   Output Base directory.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCompiler& outputBasePath( std::filesystem::path outputBasePath ) = 0;

    /**
     * @brief Updates the Media Set Name.
     *
     * The media set name is used for generating the output media set directory.
     *
     * @param[in] mediaSetName
     *   Media Set Name.
     *
     * @return *this for chaining.
     **/
    virtual FilesystemMediaSetCompiler& mediaSetName( std::string mediaSetName ) = 0;

    /** @} **/

    /**
     * @brief Executes the ARINC 665 %Media Set Compiler.
     *
     * All parameters must have been set previously.
     *
     * @return Media Set Paths relative to Output Directory Base Path.
     *
     * @throw Arinc665Exception
     *   When compilation fails
     **/
    [[nodiscard]] virtual MediaSetPaths operator()() = 0;
};

}

#endif
