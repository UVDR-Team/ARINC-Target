// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::LoadListFile.
 **/

#ifndef ARINC_665_FILES_LOADLISTFILE_HPP
#define ARINC_665_FILES_LOADLISTFILE_HPP

#include <arinc_665/files/Files.hpp>
#include <arinc_665/files/ListFile.hpp>
#include <arinc_665/files/LoadInfo.hpp>

#include <string_view>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 Load List %File (LOADS.LUM).
 *
 * @par File Format (ARINC 665-4)
 * Name of Field                   | Field Size (bits)
 * --------------------------------|:----------------:
 * LOADS.LUM File Length           | 32
 * Media File Format Version       | 16
 * Spare                           | 16
 * Pointer to Media Set PN Length  | 32
 * Pointer to Number of Loads      | 32
 * Pointer to User Defined Data    | 32
 * Expansion Point No. 1           |  0
 * Media Set PN Length             | 16
 * Media Set PN                    | 16
 * Media Sequence Number (X)       |  8
 * Number Of Media Set Members (Y) |  8
 * Number of Loads                 | 16
 * + Load Pointer                  | 16
 * + Load PN Length                | 16
 * + Load PN                       | 16
 * + Header File Name Length       | 16
 * + Header File Name              | 16
 * + Member Sequence Number        | 16
 * + Number of Target HW IDs       | 16
 * +* Target HW ID Length          | 16
 * +* Target HW ID                 | 16
 * + Expansion Point No. 2         |  0
 * Expansion Point No. 3           |  0
 * User Defined Data               | 16
 * LOADS.LUM File CRC              | 16
 **/
class ARINC_665_EXPORT LoadListFile final : public ListFile
{
  public:
    //! Offset of the Spare field (since ARINC 665-2).
    static constexpr auto SpareFieldOffsetV2{ 6Z };

    //! Offset of the Media Set Part Number Pointer Field (since ARINC 665-1)
    static constexpr auto MediaSetPartNumberPointerFieldOffsetV1{ 6Z };

    //! Offset of the Media Set Part Number Pointer Field (since ARINC 665-2)
    static constexpr auto MediaSetPartNumberPointerFieldOffsetV2{ 8Z };

    //! Offset of the Loads Pointer Field (since ARINC 665-1)
    static constexpr auto LoadFilesPointerFieldOffsetV1{ 10Z };

    //! Offset of the Loads Pointer Field (since ARINC 665-2)
    static constexpr auto LoadFilesPointerFieldOffsetV2{ 12Z };

    //! Offset of the User Defined Data Pointer Field (since ARINC 665-1)
    static constexpr auto UserDefinedDataPointerFieldOffsetV1{ 14Z };

    //! Offset of the User Defined Data Pointer Field (since ARINC 665-2)
    static constexpr auto UserDefinedDataPointerFieldOffsetV2{ 16Z };

    //! First Start of Pointer Data for ARINC 665 Load List Files (since ARINC 665-1).
    static constexpr auto FileHeaderSizeV1{ 18Z };

    //! First Start of Pointer Data for ARINC 665 Load List Files (since ARINC 665-2).
    static constexpr auto FileHeaderSizeV2{ 20Z };

    /**
     * @brief Creates an empty load list file.
     *
     * @param[in] version
     *   ARINC 665 version.
     **/
    explicit LoadListFile( SupportedArinc665Version version = SupportedArinc665Version::Supplement345 );

    /**
     * @brief Creates a load list file from the given raw data.
     *
     * @param[in] rawFile
     *   Raw data file representation.
     **/
    explicit LoadListFile( Helper::ConstRawDataSpan rawFile );

    //! Destructor.
    ~LoadListFile() override = default;

    //! @copydoc ListFile::operator=(Helper::ConstRawDataSpan)
    LoadListFile& operator=( Helper::ConstRawDataSpan rawFile ) override;

    //! @copydoc ListFile::fileType() const noexcept
    [[nodiscard]] FileType fileType() const noexcept override;

    /**
     * @name Loads
     * @{
     **/

    /**
     * @brief Returns the number of loads.
     *
     * @return Number of Loads.
     **/
    [[nodiscard]] size_t numberOfLoads() const;

    /**
     * @brief Returns the Loads.
     *
     * @return Loads.
     **/
    [[nodiscard]] const LoadsInfo& loads() const;

    /**
     * @brief Returns the Loads.
     *
     * @return Loads.
     **/
    [[nodiscard]] LoadsInfo& loads();

    /**
     * @brief Updates the Loads Information.
     *
     * @param[in] loadsInformation
     *   Loads Information.
     **/
    void loads( LoadsInfo loadsInformation );

    /**
     * @brief Adds the given load information.
     *
     * @param[in] load
     *   Load information.
     **/
    void load( LoadInfo load );

    /** @} **/

    /**
     * @name User Defined Data
     * @{
     **/

    /**
     * @brief Returns the user defined data.
     *
     * @return User Defined Data.
     **/
    [[nodiscard]] Helper::ConstRawDataSpan userDefinedData() const;

    /**
     * @brief Updates the user defined data.
     *
     * @note The user defined data must be multiple of 2-bytes in size.
     *
     * @param[in] userDefinedData
     *   User Defined Data.
     **/
    void userDefinedData( Helper::RawData userDefinedData );

    /** @} **/

    /**
     * @brief Returns if the given load list file belongs to the same media set.
     *
     * Compares:
     * - Part number,
     * - number of media set members,
     * - the load list, and
     * - the user-defined data
     * for match.
     *
     * @param[in] other
     *   The other load list file.
     *
     * @return If the given load list files belongs to the same media set.
     **/
    [[nodiscard]] bool belongsToSameMediaSet( const LoadListFile &other ) const;

  private:
    //! @copydoc ListFile::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the body of the batch file.
     *
     * @param[in] rawFile
     *   Raw load list file representation.
     **/
    void decodeBody( Helper::ConstRawDataSpan rawFile );

    /**
     * @brief Encodes the batches information list.
     *
     * @return Raw representation of loads information list.
     **/
    [[nodiscard]] Helper::RawData encodeLoadsInfo() const;

    /**
     * @brief Decodes the loads information list from the raw data.
     *
     * @param[in] rawData
     *   Raw loads info representation.
     *
     * @throw InvalidArinc665File When member sequence number is out of range
     **/
    void decodeLoadsInfo( Helper::ConstRawDataSpan rawData );

    /**
     * @brief Checks, if the User Defined Data is a multiple of 2 size.
     **/
    void checkUserDefinedData();

    //! Load List
    LoadsInfo loadsV;
    //! User Defined Data.
    Helper::RawData userDefinedDataV;
};

}

#endif
