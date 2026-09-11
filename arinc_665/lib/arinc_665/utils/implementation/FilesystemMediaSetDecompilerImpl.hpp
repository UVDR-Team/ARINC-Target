// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::FilesystemMediaSetDecompilerImpl.
 **/

#ifndef ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMMEDIASETDECOMPILERIMPL_HPP
#define ARINC_665_UTILS_IMPLEMENTATION_FILESYSTEMMEDIASETDECOMPILERIMPL_HPP

#include <arinc_665/utils/FilesystemMediaSetDecompiler.hpp>

namespace Arinc665::Utils {

/**
 * @brief Implementation of a Filesystem ARINC 665 Media Set Decompiler.
 **/
class FilesystemMediaSetDecompilerImpl final : public FilesystemMediaSetDecompiler
{
  public:
    /**
     * @brief Initialises the ARINC 665 Media Set Decompiler.
     **/
    explicit FilesystemMediaSetDecompilerImpl();

    ~FilesystemMediaSetDecompilerImpl() override;

    //! @copydoc FilesystemMediaSetDecompiler::progressHandler()
    FilesystemMediaSetDecompiler& progressHandler( MediaSetDecompiler::ProgressHandler progressHandler ) override;

    //! @copydoc FilesystemMediaSetDecompiler::checkFileIntegrity()
    FilesystemMediaSetDecompiler& checkFileIntegrity( bool checkFileIntegrity ) noexcept override;

    //! @copydoc FilesystemMediaSetDecompiler::mediaPaths()
    FilesystemMediaSetDecompiler& mediaPaths( MediaPaths mediaPaths ) override;

    /**
     * @brief Entry-point of the ARINC 665 Media Set Decompiler.
     *
     * @return Created Media Set Instance.
     **/
    [[nodiscard]] MediaSetDecompilerResult operator()() override;

  private:
    /**
     * @brief Returns File Size of the given File.
     *
     * @param[in] mediumNumber
     *   Medium Number.
     * @param[in] path
     *   Path of file on %Medium.
     *
     * @return File Size
     **/
    [[nodiscard]] size_t getFileSize( const MediumNumber &mediumNumber, const std::filesystem::path &path );

    /**
     * @brief Reads the give file and returns the data.
     *
     * @param[in] mediumNumber
     *   Medium number.
     * @param[in] path
     *   Path of the file on Medium.
     *
     * @return Read file data.
     *
     * @throw Arinc665Exception
     *   If the file does not exist or cannot be read.
     **/
    [[nodiscard]] Helper::RawData readFile( const MediumNumber &mediumNumber, const std::filesystem::path &path );

    //! Media Set Decompiler
    MediaSetDecompilerPtr mediaSetDecompilerV;
    //! Media Paths
    MediaPaths mediaPathsV;
};

}

#endif
