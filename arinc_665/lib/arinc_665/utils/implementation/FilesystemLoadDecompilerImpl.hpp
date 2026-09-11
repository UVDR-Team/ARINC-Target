// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemLoadDecompilerImpl.
 **/

#ifndef ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMLOADDECOMPILERIMPL_HPP
#define ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMLOADDECOMPILERIMPL_HPP

#include <arinc_665/utils/FilesystemLoadDecompiler.hpp>

#include <arinc_665/files/LoadHeaderFile.hpp>

namespace Arinc665::Utils {

/**
 * @brief Implementation of FilesystemLoadDecompiler for loading ARINC 665 files from the filesystem.
 **/
class FilesystemLoadDecompilerImpl final : public FilesystemLoadDecompiler
{
  public:
    explicit FilesystemLoadDecompilerImpl();

    //! @copydoc FilesystemLoadDecompiler::checkFileIntegrity()
    FilesystemLoadDecompiler& checkFileIntegrity( bool checkFileIntegrity ) noexcept override;

    //! @copydoc FilesystemLoadDecompiler::loadPath()
    FilesystemLoadDecompiler& loadPath( std::filesystem::path path ) override;

    /**
     *
     * @return
     **/
    [[nodiscard]] LoadDecompilerResult operator ()() override;

  private:
    /**
     * @brief Returns the Load Header relative File Path.
     *
     * @param[in] filename
     *   Filename.
     *
     * @return Relative path of the given file.
     **/
    std::filesystem::path filePath( std::string_view filename ) const;

    /**
     * @brief Returns File Size of the given File.
     *
     * @param[in] path
     *   Relative Path of the file.
     *
     * @return File Size
     **/
    [[nodiscard]] size_t fileSize( const std::filesystem::path &path );

    /**
     * @brief Reads the give file and returns the data.
     *
     * @param[in] path
     *   Relative Path of the file.
     *
     * @return Read file data.
     *
     * @throw Arinc665Exception
     *   If the file does not exist or cannot be read.
     **/
    [[nodiscard]] Helper::RawData readFile( const std::filesystem::path &path );

    //! Load Decompiler
    LoadDecompilerPtr loadDecompilerV;
    //! Load Base Path
    std::filesystem::path loadBasePathV;
};

}

#endif
