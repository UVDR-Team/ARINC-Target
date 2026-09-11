// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemMediaSetCompilerImpl.
 **/

#ifndef ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMMEDIASETCOMPILERIMPL_HPP
#define ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMMEDIASETCOMPILERIMPL_HPP

#include <arinc_665/utils/FilesystemMediaSetCompiler.hpp>

#include <helper/RawData.hpp>

namespace Arinc665::Utils {

/**
 * @brief Implementation of a Filesystem ARINC 665 Media Set Compiler.
 **/
class FilesystemMediaSetCompilerImpl final : public FilesystemMediaSetCompiler
{
  public:
    /**
     * @brief Initialises the ARINC 665 %Media Set Compiler.
     **/
    FilesystemMediaSetCompilerImpl();

    ~FilesystemMediaSetCompilerImpl() override;

    //! @copydoc FilesystemMediaSetCompiler::mediaSet()
    FilesystemMediaSetCompiler &mediaSet( Media::ConstMediaSetPtr mediaSet ) override;

    //! @copydoc FilesystemMediaSetCompiler::arinc665Version()
    FilesystemMediaSetCompiler &arinc665Version( SupportedArinc665Version version ) override;

    //! @copydoc FilesystemMediaSetCompiler::createBatchFiles()
    FilesystemMediaSetCompiler &createBatchFiles( FileCreationPolicy createBatchFiles ) override;

    //! @copydoc FilesystemMediaSetCompiler::createLoadHeaderFiles()
    FilesystemMediaSetCompiler &createLoadHeaderFiles( FileCreationPolicy createLoadHeaderFiles ) override;

    //! @copydoc FilesystemMediaSetCompiler::sourceBasePath()
    FilesystemMediaSetCompiler &sourceBasePath( std::filesystem::path sourceBasePath ) override;

    //! @copydoc FilesystemMediaSetCompiler::filePathMapping()
    FilesystemMediaSetCompiler &filePathMapping( FilePathMapping filePathMapping ) override;

    //! @copydoc FilesystemMediaSetCompiler::outputBasePath()
    FilesystemMediaSetCompiler &outputBasePath( std::filesystem::path outputBasePath ) override;

    //! @copydoc FilesystemMediaSetCompiler::mediaSetName()
    FilesystemMediaSetCompiler &mediaSetName( std::string mediaSetName ) override;

    /**
     * @brief Entry-point of the Filesystem ARINC 665 Media Set Compiler.
     ***/
    [[nodiscard]] MediaSetPaths operator()() override;

  private:
    /**
     * @brief Returns the medium path.
     *
     * @param[in] mediumNumber
     *   Medium Number.
     *
     * @return Medium Path.
     **/
    [[nodiscard]] std::filesystem::path mediumPath( const Arinc665::MediumNumber &mediumNumber ) const;

    /**
     * @brief Creates the Directory for the given Medium.
     *
     * @param[in] mediumNumber
     *   Medium to Create.
     **/
    void createMedium( const MediumNumber &mediumNumber );

    /**
     * @brief Creates the given Directory.
     *
     * @param[in] mediumNumber
     *   Medium Number, where to create the directory in.
     * @param[in] directory
     *   Directory to Create.
     **/
    void createDirectory( const MediumNumber &mediumNumber, const Media::ConstDirectoryPtr &directory );

    /**
     * @brief Check File Existence Handler.
     *
     * @param[in] file
     *   File to Check
     *
     * @return If the @p file exists
     **/
    [[nodiscard]] bool checkFileExistence( const Media::ConstFilePtr &file );

    /**
     * @brief Create File Handler.
     *
     * Copies the file from source to destination.
     *
     * @param[in] file
     *   File to Create
     **/
    void createFile( const Media::ConstFilePtr &file );

    /**
     * @brief Write File Handler
     *
     * @param[in] mediumNumber
     *   Medium Number
     * @param[in] path
     *   File Path on %Medium
     * @param[in] file
     *   File Content
     **/
    void writeFile(
      const MediumNumber &mediumNumber,
      const std::filesystem::path &path,
      Helper::ConstRawDataSpan file );

    /**
     * @brief Read File Handler
     *
     * @param[in] mediumNumber
     *   Medium number.
     * @param[in] path
     *   File Path
     *
     * @return File Content
     **/
    [[nodiscard]] Helper::RawData readFile( const MediumNumber &mediumNumber, const std::filesystem::path &path );

    //! Media Set Compiler
    MediaSetCompilerPtr mediaSetCompilerV;
    //! Source Base Path
    std::filesystem::path sourceBasePathV;
    //! File Path Mapping
    FilePathMapping filePathMappingV;
    //! Output Base Path
    std::filesystem::path outputBasePathV;
    //! Media Set Name
    std::string mediaSetNameV;
    //! Media Set Base Directory
    std::filesystem::path mediaSetBaseDirectoryV;
    //! Generated Media Paths
    MediaPaths mediaPathsV;
};

}

#endif
