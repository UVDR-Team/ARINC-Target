// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::FileListFile.
 **/

#ifndef ARINC_665_FILES_FILELISTFILE_HPP
#define ARINC_665_FILES_FILELISTFILE_HPP

#include <arinc_665/files/Files.hpp>
#include <arinc_665/files/ListFile.hpp>
#include <arinc_665/files/FileInfo.hpp>

#include <arinc_649/Arinc649.hpp>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 %File List %File (FILES.LUM).
 *
 * @par File Format (ARINC 665-4)
 * Name of Field                                | Field Size (bits)
 * ---------------------------------------------|:----------------:
 * FILES.LUM File Length                        | 32
 * Media File Format Version                    | 16
 * Spare                                        | 16
 * Pointer to Media Set PN Length               | 32
 * Pointer to Number of Media Set Files         | 32
 * Pointer To User Defined Data                 | 32
 * Pointer to FILES.LUM File Check Value Length | 32
 * Expansion Point No. 1                        |  0
 * Media Set PN Length                          | 16
 * Media Set PN                                 | 16
 * Media Sequence Number (X)                    |  8
 * No. Of Media Set Members (Y)                 |  8
 * Number of Media Set Files                    | 16
 * # File Pointer                               | 16
 * # File Name Length                           | 16
 * # File Name                                  | 16
 * # File Pathname Length                       | 16
 * # File Pathname                              | 16
 * # File Member Sequence No.                   | 16
 * # File CRC                                   | 16
 * # File Check Value Length                    | 16
 * # File Check Value Type                      | 16
 * # File Check Value                           | 16
 * # Expansion Point No. 2                      |  0
 * Expansion Point No. 3                        |  0
 * User Defined Data                            | 16
 * FILES.LUM File Check Value Length            | 16
 * FILES.LUM File Check Value Type              | 16
 * FILES.LUM File Check Value                   | 16
 * FILES.LUM File CRC                           | 16
 **/
class ARINC_665_EXPORT FileListFile final : public ListFile
{
  public:
    //! Offset of the Spare field (Since ARINC 665-2).
    static constexpr ptrdiff_t SpareFieldOffsetV2{ 6 };

    //! Offset of the Media Set Part Number Pointer Field (Since ARINC 665-1)
    static constexpr ptrdiff_t MediaSetPartNumberPointerFieldOffsetV1{ 6 };

    //! Offset of the Media Set Part Number Pointer Field (Since ARINC 665-2)
    static constexpr ptrdiff_t MediaSetPartNumberPointerFieldOffsetV2{ 8 };

    //! Offset of the Media Set Files Pointer Field (Since ARINC 665-1)
    static constexpr ptrdiff_t MediaSetFilesPointerFieldOffsetV1{ 10 };

    //! Offset of the Media Set Files Pointer Field (Since ARINC 665-2)
    static constexpr ptrdiff_t MediaSetFilesPointerFieldOffsetV2{ 12 };

    //! Offset of the User Defined Data Pointer Field (Since ARINC 665-1)
    static constexpr ptrdiff_t UserDefinedDataPointerFieldOffsetV1{ 14 };

    //! Offset of the User Defined Data Pointer Field (Since ARINC 665-2)
    static constexpr ptrdiff_t UserDefinedDataPointerFieldOffsetV2{ 16 };

    //! Offset of the File Check Value Pointer Field (Only ARINC 665-3/4)
    static constexpr ptrdiff_t FileCheckValuePointerFieldOffsetV3{ 20 };

    //! First Start of pointer Data for ARINC 665-1 File List File.
    static constexpr ptrdiff_t FileHeaderSizeV1{ 18 };

    //! First Start of pointer Data for ARINC 665-2 File List File.
    static constexpr ptrdiff_t FileHeaderSizeV2{ 20 };

    //! First Start of pointer Data for ARINC 665-3/4/5 File List File.
    static constexpr ptrdiff_t FileHeaderSizeV3{ 24 };

    /**
     * @brief Creates an empty file list file.
     *
     * @param[in] version
     *   ARINC 665 version.
     **/
    explicit FileListFile( SupportedArinc665Version version = SupportedArinc665Version::Supplement345 );

    /**
     * @brief Creates a file list file from the given raw data.
     *
     * @param[in] rawFile
     *   Raw data file representation.
     **/
    explicit FileListFile( Helper::ConstRawDataSpan rawFile );

    //! Destructor.
    ~FileListFile() override = default;

    //! @copydoc ListFile::operator=(Helper::ConstRawDataSpan)
    FileListFile& operator=( Helper::ConstRawDataSpan rawFile ) override;

    //! @copydoc ListFile::fileType() const noexcept
    [[nodiscard]] FileType fileType() const noexcept override;

    /**
     * @name Files
     * @sa FileInfo, FilesInfo
     * @{
     **/

    /**
     * @brief Returns the number of files.
     *
     * @return Number of files.
     **/
    [[nodiscard]] size_t numberOfFiles() const;

    /**
     * @brief Return the list of files.
     *
     * @return list of files
     **/
    [[nodiscard]] const FilesInfo& files() const;

    /**
     * @brief Return the list of files.
     *
     * @return List of files
     **/
    [[nodiscard]] FilesInfo& files();

    /**
     * @brief Replaces the List of Files with @p files.
     *
     * @param[in] files
     *   New File List
     **/
    void files( FilesInfo files );

    /**
     * @brief Adds the given file information.
     *
     * @param[in] file
     *   File information.
     **/
    void file( FileInfo file );

    /** @} **/

    /**
     * @name User Defined Data
     * @{
     **/

    /**
     * @brief Returns the User Defined Data.
     *
     * @return User Defined Data.
     **/
    [[nodiscard]] Helper::ConstRawDataSpan userDefinedData() const;

    /**
     * @brief Updates the User Defined Data.
     *
     * @param[in] userDefinedData
     *   User Defined Data.
     **/
    void userDefinedData( Helper::RawData userDefinedData );

    /** @} **/

    /**
     * @name Check Value Type
     *
     * Check Value Type used for Generation of the File List File
     * @{
     **/

    /**
     * @brief Returns the Check Value Type.
     *
     * @return Check Value.
     **/
    [[nodiscard]] Arinc649::CheckValueType checkValueType() const;

    /**
     * @brief Updates the Check Value Type
     *
     * @param[in] type
     *   Check Value Type.
     **/
    void checkValueType( Arinc649::CheckValueType type );

    /** @} **/

    /**
     * @brief Returns if the given file list file belongs to the same media set.
     *
     * @param[in] other
     *   The other file list file to compare to this.
     *
     * @return If the given file list file belongs to the same media set.
     **/
    [[nodiscard]] bool belongsToSameMediaSet( const FileListFile &other ) const;

  private:
    //! @copydoc ListFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the body of the file list file.
     *
     * @param[in] rawFile
     *   Raw file list file representation.
     **/
    void decodeBody( Helper::ConstRawDataSpan rawFile );

    /**
     * @brief Encodes the files information list.
     *
     * @param[in] encodeV3Data
     *   If set to true, additional data as stated in ARINC 665-3 is encoded.
     *
     * @return Raw representation of files information list.
     **/
    [[nodiscard]] Helper::RawData encodeFilesInfo( bool encodeV3Data ) const;

    /**
     * @brief Decodes the files information list from the raw data.
     *
     * @param[in] rawData
     *   Raw files list information representation.
     * @param[in] decodeV3Data
     *   If set to true, additional data as stated in ARINC 665-3 is decoded.
     **/
    void decodeFilesInfo( Helper::ConstRawDataSpan rawData, bool decodeV3Data );

    /**
     * @brief Checks if the User Defined Data size is a multiple of 2.
     **/
    void checkUserDefinedData();

    //! Files Information (list)
    FilesInfo filesV;
    //! User Defined Data.
    Helper::RawData userDefinedDataV;
    //! Check Value Type (since ARINC 665-3)
    Arinc649::CheckValueType checkValueTypeV{ Arinc649::CheckValueType::NotUsed };
};

}

#endif
