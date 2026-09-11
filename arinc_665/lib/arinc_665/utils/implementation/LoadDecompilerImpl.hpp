// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::LoadDecompilerImpl.
 **/

#ifndef ARINC_665_UTILS_IMPLEMENTATION_LOADDECOMPILERIMPL_HPP
#define ARINC_665_UTILS_IMPLEMENTATION_LOADDECOMPILERIMPL_HPP

#include <arinc_665/utils/LoadDecompiler.hpp>

#include <arinc_665/files/LoadHeaderFile.hpp>

namespace Arinc665::Utils {

/**
 * @brief Implementation of Load Decompiler.
 **/
class LoadDecompilerImpl final : public LoadDecompiler
{
  public:
    explicit LoadDecompilerImpl() = default;

    //! @copydoc LoadDecompiler::filePathHandler()
    LoadDecompiler& filePathHandler( FilePathHandler handler ) override;

    //! @copydoc LoadDecompiler::fileSizeHandler()
    LoadDecompiler& fileSizeHandler( FileSizeHandler handler ) override;

    //! @copydoc LoadDecompiler::readFileHandler()
    LoadDecompiler& readFileHandler( ReadFileHandler handler ) override;

    //! @copydoc LoadDecompiler::checkFileIntegrity()
    LoadDecompiler& checkFileIntegrity( bool checkFileIntegrity ) noexcept override;

    //! @copydoc LoadDecompiler::loadFilename()
    LoadDecompiler& loadFilename( std::string_view filename ) override;

    /**
     * @brief Entry-point of the ARINC 665 Load Decompiler.
     *
     * @return Created Load Instance.
     **/
    [[nodiscard]] LoadDecompilerResult operator()() override;

  private:
    /**
     * @brief Loads and processes a decompiler file, extracting and validating its components.
     *
     * This method is responsible for the processing of a load file, which includes its addition to a media set,
     * decoding its header, verifying its integrity, and handling associated data and support files.
     **/
    void loadFile();

    /**
     * @brief Creates the logical directory entry if not already created and return its representation.
     *
     * @param[in] directoryPath
     *   Directory Path.
     *
     * @return Directory entry.
     *
     * @throw Arinc665Exception
     *   When the subdirectory cannot be created
     **/
    [[nodiscard]] Media::ContainerEntityPtr checkCreateDirectory( const std::filesystem::path &directoryPath );

    /**
    * @brief Preform Checks of Load Files (data and support).
    *
    * Calculates Load CRC and Load Check Value.
    * Verifies File Length.
    * Verifies File CRC and File Check Value.
    *
    * @param[in,out] loadCrc
    *   Load CRC
    * @param[in,out] loadCheckValueGenerator
    *   Load Check Value Generator
    * @param[in] filePath
    *   File path
    * @param[in] loadFileInfo
    *   Load File Information
    * @param[in] fileSize16Bit
    *   If Data Size is stored in multiple of 16bit.
    *   This is true for data files in ARINC 665-2 Load Header Files.
    *
    * @throw Arinc665Exception
    *   If File Integrity checks failed.
    **/
    void checkLoadFile(
      Arinc649::Arinc649Crc32 &loadCrc,
      Arinc649::CheckValueGenerator &loadCheckValueGenerator,
      const std::filesystem::path &filePath,
      const Files::LoadFileInfo &loadFileInfo,
      bool fileSize16Bit ) const;

    //! File Path Handler
    FilePathHandler filePathHandlerV;
    //! File Size Handler
    FileSizeHandler fileSizeHandlerV;
    //! Read File Handler
    ReadFileHandler readFileHandlerV;
    //! Check File Integrity
    bool checkFileIntegrityV{ true };
    //! Load Filename
    std::string loadFilenameV;

    //! Load
    Media::LoadPtr loadV;
    //! Media Set
    Media::MediaSetPtr mediaSetV;
    //! Check Values
    Media::CheckValues checkValuesV;
};

}

#endif
