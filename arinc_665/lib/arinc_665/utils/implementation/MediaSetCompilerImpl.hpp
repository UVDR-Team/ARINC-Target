// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::MediaSetCompilerImpl.
 **/

#ifndef ARINC_665_UTILS_IMPLEMENTATION_MEDIASETCOMPILERIMPL_HPP
#define ARINC_665_UTILS_IMPLEMENTATION_MEDIASETCOMPILERIMPL_HPP

#include <arinc_665/utils/MediaSetCompiler.hpp>

namespace Arinc665::Utils {

/**
 * @brief Implementation of a %Media Set Compiler.
 **/
class MediaSetCompilerImpl final : public MediaSetCompiler
{
  public:
    /**
     * @brief Initialises the ARINC 665 %Media Set Compiler
     **/
    MediaSetCompilerImpl() = default;

    //! @copydoc MediaSetCompiler::mediaSet()
    MediaSetCompiler& mediaSet( Media::ConstMediaSetPtr mediaSet ) override;

    //! @copydoc MediaSetCompiler::createMediumHandler()
    MediaSetCompiler &createMediumHandler( CreateMediumHandler createMediumHandler ) override;

    //! @copydoc MediaSetCompiler::createDirectoryHandler()
    MediaSetCompiler &createDirectoryHandler( CreateDirectoryHandler createDirectoryHandler ) override;

    //! @copydoc MediaSetCompiler::checkFileExistenceHandler()
    MediaSetCompiler &checkFileExistenceHandler( CheckFileExistenceHandler checkFileExistenceHandler ) override;

    //! @copydoc MediaSetCompiler::createFileHandler()
    MediaSetCompiler &createFileHandler( CreateFileHandler createFileHandler ) override;

    //! @copydoc MediaSetCompiler::writeFileHandler()
    MediaSetCompiler &writeFileHandler( WriteFileHandler writeFileHandler ) override;

    //! @copydoc MediaSetCompiler::readFileHandler()
    MediaSetCompiler &readFileHandler( ReadFileHandler readFileHandler ) override;

    //! @copydoc MediaSetCompiler::arinc665Version()
    MediaSetCompiler &arinc665Version( SupportedArinc665Version version ) override;

    //! @copydoc MediaSetCompiler::createBatchFiles()
    MediaSetCompiler &createBatchFiles( FileCreationPolicy createBatchFiles ) override;

    //! @copydoc MediaSetCompiler::createLoadHeaderFiles()
    MediaSetCompiler &createLoadHeaderFiles( FileCreationPolicy createLoadHeaderFiles ) override;

    /**
     * @brief Entry-point of the ARINC 665 Media Set Exporter.
     ***/
    void operator()() override;

  private:
    /**
     * @brief Called to export the given Directory.
     *
     * Exports all subdirectories, files, loads and batches.
     *
     * @param[in] directory
     *   Directory, which is exported.
     * @param[in] mediumNumber
     *   Medium Number.
     **/
    void exportDirectory( const MediumNumber &mediumNumber, const Media::ConstDirectoryPtr &directory );

    /**
     * @brief Called to export the given Regular File.
     *
     * @param[in] file
     *   File, which is exported.
     **/
    void exportRegularFile( const Media::ConstRegularFilePtr &file );

    /**
     * @brief Called to export the given Load Header File.
     *
     * Depending on the file type and Load File Creation Policy, the file create callback is called or the load header
     * is created.
     *
     * @param[in] load
     *   Load, which is exported.
     **/
    void exportLoad( const Media::ConstLoadPtr &load );

    /**
     * @brief Called to export the given Batch File.
     *
     * Depending on the file type and Batches File Creation Policy, the file create callback is called or the batch is
     * created.
     *
     * @param[in] batch
     *   Batch, which is exported.
     **/
    void exportBatch( const Media::ConstBatchPtr &batch );

    /**
     * @brief Exports List of Loads File.
     **/
    void exportListOfLoads() const;

    /**
     * @brief Exports List of Batches File.
     **/
    void exportListOfBatches() const;

    /**
     * @brief Exports List of Files File.
     **/
    void exportListOfFiles() const;

    /**
     * @brief Creates the Load Header File.
     *
     * @param[in] load
     *   Load header file.
     **/
    void createLoadHeaderFile( const Media::Load &load ) const;

    /**
     * @brief Calculate Load file CRC, Check Value and returns File Information.
     *
     * This operation is used to fill in the data and support file information within the load header.
     *
     * @param[in] loadFile
     *   Load File.
     *
     * @return Processed Load File Information
     **/
    [[nodiscard]] Files::LoadFileInfo loadFileInformation( const Media::ConstLoadFile &loadFile ) const;

    /**
     * @brief Creates the Batch File.
     *
     * @param[in] batch
     *   Batch file.
     **/
    void createBatchFile( const Media::Batch &batch ) const;

    /**
     * @brief Calculates CRC-16 and Check Value of the given file.
     *
     * The file is read with the @ref readFileHandler() from the output medium.
     * This operation is alo used for Check Value Generation of Load List and Batches List.
     *
     * @param[in] mediumNumber
     *   Medium Number
     * @param[in] filename
     *   Filename.
     * @param[in] checkValueType
     *   Desired Check Value Type.
     *
     * @return CRC16 and Check Value as std::tuple.
     **/
    [[nodiscard]] std::tuple< uint16_t, Arinc649::CheckValue > fileCrcCheckValue(
      MediumNumber mediumNumber,
      const std::filesystem::path &filename,
      Arinc649::CheckValueType checkValueType ) const;

    //! ARINC 665 Version used for exporting
    SupportedArinc665Version arinc665VersionV{ SupportedArinc665Version::Supplement2 };
    //! Indicates if batch files shall be created by Media set Exporter
    FileCreationPolicy createBatchFilesV{ FileCreationPolicy::None };
    //! Indicates if load header files shall be created by Media Set Exporter
    FileCreationPolicy createLoadHeaderFilesV{ FileCreationPolicy::None };

    //! Media Set
    Media::ConstMediaSetPtr mediaSetV;
    //! Create Medium Handler
    CreateMediumHandler createMediumHandlerV;
    //! Create Directory Handler
    CreateDirectoryHandler createDirectoryHandlerV;
    //! Check File existence Handler
    CheckFileExistenceHandler checkFileExistenceHandlerV;
    //! Create File Handler
    CreateFileHandler createFileHandlerV;
    //! Write File Handler
    WriteFileHandler writeFileHandlerV;
    //! Read File Handler
    ReadFileHandler readFileHandlerV;
};

}

#endif
