// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::BatchFile.
 **/

#ifndef ARINC_665_FILES_BATCH_HPP
#define ARINC_665_FILES_BATCH_HPP

#include <arinc_665/files/Files.hpp>
#include <arinc_665/files/Arinc665File.hpp>
#include <arinc_665/files/BatchTargetInfo.hpp>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 Batch %File (*.LUB).
 *
 * @par File Format (ARINC 665-4)
 * Name of Field                                      | Field Size (bits)
 * ---------------------------------------------------|:-----------------:
 * Batch File Length                                  | 32
 * Batch File Format Version                          | 16
 * Spare                                              | 16
 * Pointer to Batch File PN Length                    | 32
 * Pointer to Number of Target HW ID Load-List Blocks | 32
 * Expansion Point 1                                  |  0
 * Batch File PN Length                               | 16
 * Batch File PN                                      | 16
 * Comment Length                                     | 16
 * Comment                                            | 16
 * Expansion Point 2                                  |  0
 * Number of Target HW ID Load-List Blocks            | 16
 * + Pointer to Next Target HW ID Load-List Block     | 16
 * + Target HW ID POS Length                          | 16
 * + Target HW ID POS                                 | 16
 * + Number of Loads for Target HW ID POS             | 16
 * +# Header File Name Length                         | 16
 * +# Header File Name                                | 16
 * +# Load PN Length                                  | 16
 * +# Load PN                                         | 16
 * Expansion Point 3                                  |  0
 * Batch File CRC                                     | 16
 **/
class ARINC_665_EXPORT BatchFile final : public Arinc665File
{
  public:
    ///! Offset of the Spare field (since ARINC 665-2).
    static constexpr std::ptrdiff_t SpareFieldOffsetV2{ 6 };

    //! Offset of the Batch Part Number Pointer Field (since ARINC 665-1)
    static constexpr std::ptrdiff_t BatchPartNumberPointerFieldOffsetV1{ 6 };

    //! Offset of the Batch Part Number Pointer Field (since ARINC 665-2)
    static constexpr std::ptrdiff_t BatchPartNumberPointerFieldOffsetV2{ 8 };

    //! Offset of the THW IDs Pointer Field (since ARINC 665-1)
    static constexpr std::ptrdiff_t ThwIdsPointerFieldOffsetV1{ 10 };

    //! Offset of the THW IDs Pointer Field (since ARINC 665-2)
    static constexpr std::ptrdiff_t ThwIdsPointerFieldOffsetV2{ 12 };

    //! First Start of Pointer Data for ARINC 665-1 Batch Files.
    static constexpr std::ptrdiff_t BatchFileHeaderSizeV1{ 14 };

    //! First Start of Pointer Data for ARINC 665-2 Batch Files.
    static constexpr std::ptrdiff_t BatchFileHeaderSizeV2{ 16 };

    /**
     * @brief Creates an empty Batch File.
     *
     * @param[in] version
     *   ARINC 665 Version.
     **/
    explicit BatchFile( SupportedArinc665Version version = SupportedArinc665Version::Supplement345 );

    /**
     * @brief Creates a batch file from the given raw data.
     *
     * @param[in] rawFile
     *   Raw data file representation.
     **/
    explicit BatchFile( Helper::ConstRawDataSpan rawFile );

    //! Destructor.
    ~BatchFile() override = default;

    //! @copydoc Arinc665File::operator=
    BatchFile& operator=( Helper::ConstRawDataSpan rawFile ) override;

    //! @copydoc Arinc665File::fileType() const noexcept
    [[nodiscard]] FileType fileType() const noexcept override;

    /**
     * @name Batch Part Number
     *
     * A Batch is assigned a Part Number.
     *
     * @{
     **/

    /**
     * @brief Returns the Part Number of the Batch.
     *
     * @return Part Number of the Batch.
     **/
    [[nodiscard]] std::string_view partNumber() const;

    /**
     * @brief Updates the Part Number of the Batch.
     *
     * @param[in] partNumber
     *   New Batch Part Number.
     **/
    void partNumber( std::string partNumber );

    /** @} **/

    /**
     * @name Comment
     *
     * Comment, which describes the Batch.
     *
     * @{
     **/

    /**
     * @brief Returns the Comment text of the Batch.
     *
     * @return Comment text of the batch file.
     **/
    [[nodiscard]] std::string_view comment() const;

    /**
     * @brief Updates the comment text of the Batch.
     *
     * @param[in] comment
     *   New comment text.
     **/
    void comment( std::string comment );

    /** @} **/

    /**
     * @name Batch Targets Hardware Information
     * @sa BatchTargetInfo, BatchTargetsInfo
     * @{
     **/

    /**
     * @brief Returns the Targets Hardware Information.
     *
     * @return Targets Hardware Information.
     **/
    [[nodiscard]] const BatchTargetsInfo& targetsHardware() const;

    //! @copydoc targetsHardware() const
    [[nodiscard]] BatchTargetsInfo& targetsHardware();

    /**
     * @brief Adds the given target hardware information to the batch file.
     *
     * @param[in] targetHardwareInfo
     *   Target Hardware Information to add.
     **/
    void targetHardware( BatchTargetInfo targetHardwareInfo );

    /** @} **/

  private:
    //! @copydoc Arinc665File::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the body of the batch file.
     *
     * @param[in] rawFile
     *   Raw batch file representation.
     **/
    void decodeBody( Helper::ConstRawDataSpan rawFile );

    /**
     * @brief Encodes the target hardware information list.
     *
     * @return Raw representation of the _Target Hardware Information_ list.
     **/
    [[nodiscard]] Helper::RawData encodeBatchTargetsInfo() const;

    /**
     * @brief Decodes the target hardware information list from the raw data.
     *
     * @param[in] rawData
     *   Raw batch targets information representation.
     **/
    void decodeBatchTargetsInfo( Helper::ConstRawDataSpan rawData );

    //! Part Number
    std::string partNumberV;
    //! Comment
    std::string commentV;
    //! Targets Hardware Information
    BatchTargetsInfo targetsHardwareV;
};

}

#endif
