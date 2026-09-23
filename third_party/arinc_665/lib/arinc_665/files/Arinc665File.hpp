// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::Arinc665File.
 **/

#ifndef ARINC_665_FILES_ARINC665FILE_HPP
#define ARINC_665_FILES_ARINC665FILE_HPP

#include <arinc_665/files/Files.hpp>

#include <arinc_support/RawData.hpp>

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 Protocol %File.
 **/
class ARINC_665_EXPORT Arinc665File
{
  public:
    //! Base Header Size
    static constexpr ptrdiff_t BaseHeaderSize{
      sizeof( uint32_t ) +  // File length field
      sizeof( uint16_t ) }; // ARINC Version field

    //! Default Checksum Position
    static constexpr ptrdiff_t DefaultChecksumPosition{ 2 };

    //! Offset of the File Length Field
    static constexpr ptrdiff_t FileLengthFieldOffset{ 0 };

    //! Offset of the File Format Version Field
    static constexpr ptrdiff_t FileFormatVersionFieldOffset{ 4 };

    /**
     * @brief Encodes the given path for storage within ARINC 665 media set files.
     *
     * The main purpose is to replace all forward slashes '/' by backslashes '\'.
     * The path always contains a trailing '\'.
     *
     * @param[in] path
     *   Path, which should be converted.
     *
     * @return Converted Path.
     **/
    static std::string encodePath( const std::filesystem::path &path );

    /**
     * @brief Decode the file length information from the given file.
     *
     * @param[in] file
     *   Raw ARINC 665 file.
     *
     * @return Encoded file length.
     *
     * @throw InvalidArinc665File
     *   If the file size is too small to represent a valid ARINC 665 file.
     **/
    static uint32_t fileLength( ArincSupport::ConstRawDataSpan file );

    /**
     * @brief Decode the format version information from the given file.
     *
     * @param[in] file
     *   Raw ARINC 665 file.
     *
     * @return Decoded format version.
     *
     * @throw InvalidArinc665File
     *   If the file size is too small to represent a valid ARINC 665 file.
     **/
    static uint16_t formatVersion( ArincSupport::ConstRawDataSpan file );

    /**
     * @brief Calculates the checksum over the given file.
     *
     * @param[in] file
     *   Raw ARINC 665 file.
     *
     * @return Calculated checksum.
     **/
    static uint16_t calculateChecksum( ArincSupport::ConstRawDataSpan file );

    /**
     * @brief Returns the ARINC 665 file class type.
     *
     * @param[in] rawFile
     *   Raw file data.
     *
     * @return ARINC 665 file class type.
     * @retval{}
     *   When @p rawFile does not map to an ARINC 665 File Type
     **/
    static std::optional< FileClassType > fileType( ArincSupport::ConstRawDataSpan rawFile );

    /**
     * @brief Returns the load header file version for @p rawFile.
     *
     * @param[in] rawFile
     *   Raw file data.
     *
     * @return The load header file version for @p rawFile.
     * @retval LoadFileFormatVersion::Invalid
     *   When @p rawFile is not a load header file
     **/
    static std::optional< LoadFileFormatVersion > loadFileFormatVersion( ArincSupport::ConstRawDataSpan rawFile );

    /**
     * @brief Returns the batch file version for @p rawFile.
     *
     * @param[in] rawFile
     *   Raw file data.
     *
     * @return Batch file version for @p rawFile.
     * @retval {}
     *   When @p rawFile is not a batch file
     **/
    static std::optional< BatchFileFormatVersion > batchFileFormatVersion( ArincSupport::ConstRawDataSpan rawFile );

    /**
     * @brief Returns the media file version for @p rawFile
     *
     * @param[in] rawFile
     *   Raw file data.
     *
     * @return The media file version for [rawFile].
     * @retval {}
     *   When @p rawFile is not a media file
     **/
    static std::optional< MediaFileFormatVersion > mediaFileFormatVersion( ArincSupport::ConstRawDataSpan rawFile );

    /**
     * @brief Returns the Supported ARINC 665 Version for the given @p fileType and @p formatVersionField.
     *
     * @param[in] fileType
     *   File Type.
     * @param[in] formatVersionField
     *   Format version field of file.
     *
     * @return Supported ARINC 665 version.
     * @retval {}
     *   If the given information are inconsistent or the Version is not supported.
     **/
    static std::optional< SupportedArinc665Version > arinc665Version(
      FileType fileType,
      uint16_t formatVersionField ) noexcept;

    /**
     * @brief Returns the ARINC 665 version for the given @p fileType and @p arinc665Version.
     *
     * @param[in] fileType
     *   File type.
     * @param[in] arinc665Version
     *   ARINC 665 version.
     *
     * @return Format version field of file.
     **/
    static uint16_t formatVersionField( FileType fileType, SupportedArinc665Version arinc665Version ) noexcept;

    /**
     * @brief Detects the file type for the given filename.
     *
     * @param[in] filename
     *   filename.
     *
     * @return File type of @p filename.
     * @retval {}
     *   If @p filename is not a ARINC 665 file type.
     **/
    static std::optional< FileType > fileType( const std::filesystem::path &filename );

    //! Destructor
    virtual ~Arinc665File() noexcept = default;

    /**
     * @brief Assigns raw data to the file.
     *
     * @note
     * Regardless this method is marked pure, a base implementation is provided.
     * A child class needs to override this method.
     *
     * @param[in] rawFile
     *   File raw data
     *
     * @return *this
     **/
    virtual Arinc665File& operator=( ArincSupport::ConstRawDataSpan rawFile ) = 0;

    /**
     * @brief Returns the ARINC 665 file as raw data.
     *
     * @return the protocol file as raw data.
     **/
    [[nodiscard]] explicit operator ArincSupport::RawData() const;

    /**
     * @brief Returns the ARINC 665 file type.
     *
     * @return ARINC 665 file type.
     **/
    [[nodiscard]] virtual FileType fileType() const noexcept = 0;

    /**
     * @name ARINC 665 Version
     * @{
     **/

    /**
     * @brief Returns the ARINC 665 version of this file.
     *
     * @return ARINC 665 version of this file.
     **/
    [[nodiscard]] SupportedArinc665Version arincVersion() const noexcept;

    /**
     * @brief Updates the ARINC 665 version of this file.
     *
     * @param[in] version
     *   ARINC 665 version.
     **/
    void arincVersion( SupportedArinc665Version version ) noexcept;

    /** @} **/

  protected:
    /**
     * @brief Initialises the ARINC 665 file.
     *
     * @param[in] version
     *   ARINC 665 version.
     * @param[in] checksumPosition
     *   Checksum position.
     **/
    explicit Arinc665File(
      SupportedArinc665Version version = SupportedArinc665Version::Supplement345,
      ptrdiff_t checksumPosition = DefaultChecksumPosition ) noexcept;

    /**
     * @brief Initialises the ARINC 665 file from the given raw data.
     *
     * @param[in] rawFile
     *   Raw File.
     * @param[in] expectedFileType
     *   Expected file type.
     * @param[in] checksumPosition
     *   Checksum position.
     **/
    explicit Arinc665File(
      ArincSupport::ConstRawDataSpan rawFile,
      FileType expectedFileType,
      ptrdiff_t checksumPosition = DefaultChecksumPosition );

    /**
     * @brief Copy Constructor
     *
     * @param[in] other
     *   Other ARINC 665 file.
     **/
    Arinc665File( const Arinc665File &other ) noexcept = default;

    //! @copydoc Arinc665File(const Arinc665File&)
    Arinc665File( Arinc665File &&other ) noexcept = default;

    /**
     * @brief Assignment operator
     *
     * @param[in] other
     *   Other file.
     *
     * @return *this
     **/
    Arinc665File& operator=( const Arinc665File &other ) noexcept;

    //! @copydoc operator=(const Arinc665File&)
    Arinc665File& operator=( Arinc665File &&other ) noexcept;

    /**
     * @brief Encodes the ARINC 665 file as raw data.
     *
     * @return ARINC 665 file as raw data.
     **/
    [[nodiscard]] virtual ArincSupport::RawData encode() const = 0;

    /**
     * @brief Inserts the header data and file CRC into @p rawFile.
     *
     * @param[in,out] rawFile
     *   Raw file, where the header is encoded.
     * @param[in] additionalSize
     *   Additional value, which should be added to the current raw file size.
     *   Normally it is the missing file CRC field (16 bit).
     *
     * @throw InvalidArinc665File
     *   When the file is too small
     * @throw InvalidArinc665File
     *   When file size is invalid
     **/
    void insertHeader( ArincSupport::RawDataSpan rawFile, std::size_t additionalSize = sizeof( uint16_t ) ) const;

    /**
     * @brief Calculates and updates the File CRC field.
     *
     * @param[in,out] rawFile
     *   Raw file.
     **/
    void calculateFileCrc( ArincSupport::RawDataSpan rawFile ) const;

  private:
    /**
     * @brief Initialises class with the given raw data.
     *
     * @param[in] rawFile
     *   Raw File.
     * @param[in] expectedFileType
     *   Expected file type.
     *
     * @throw InvalidArinc665File
     *   When file is too small
     * @throw InvalidArinc665File
     *   When file size field is invalid
     * @throw InvalidArinc665File
     *   When file format is wrong
     * @throw InvalidArinc665File
     *   When CRC is invalid
     **/
    void decodeHeader( ArincSupport::ConstRawDataSpan rawFile, FileType expectedFileType );

    //! Checksum Position
    const ptrdiff_t checksumPosition;
    //! ARINC 665 Version
    SupportedArinc665Version arinc665VersionV;
};

}

#endif
