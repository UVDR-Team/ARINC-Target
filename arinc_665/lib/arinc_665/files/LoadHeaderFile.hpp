// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::LoadHeaderFile.
 **/

#ifndef ARINC_665_FILES_LOADHEADERFILE_HPP
#define ARINC_665_FILES_LOADHEADERFILE_HPP

#include <arinc_665/files/Files.hpp>
#include <cstddef>
#include <arinc_665/files/Arinc665File.hpp>
#include <arinc_665/files/LoadFileInfo.hpp>

#include <arinc_649/Arinc649.hpp>
#include <arinc_649/Arinc649Crc.hpp>

#include <cstdint>
#include <list>
#include <optional>
#include <string_view>
#include <utility>

namespace Arinc665::Files {

/**
 * @brief ARINC 665 Load Header %File (*.LUH).
 *
 * An LSP consists of a Header File plus one or more Data Files.
 * An LSP may also include support files as needed.
 *
 * @par File Format (ARINC 665-4)
 * Name of Field                                    | Field Size (bits)
 * -------------------------------------------------|:----------------:
 * Header File Length                               | 32
 * Load File Format Version                         | 16
 * Part Flags                                       | 16
 * Pointer to Load PN Length                        | 32
 * Pointer to Number of Target HW IDs               | 32
 * Pointer to Number of Data Files                  | 32
 * Pointer to Number of Support Files               | 32
 * Pointer to User Defined Data                     | 32
 * Pointer to Load Type Description Length          | 32
 * Pointer to Number of Target HW ID with Positions | 32
 * Pointer to Load Check Value Length               | 32
 * Expansion Point No.1                             |  0
 * Load PN Length                                   | 16
 * Load PN                                          | 16
 * Expansion Point No. 2                            |  0
 * Load Type Description Length                     | 16
 * Load Type Description                            | 16
 * Load Type ID                                     | 16
 * Expansion Point No. 3                            |  0
 * Number of Target HW IDs                          | 16
 * * Target HW ID Length                            | 16
 * * Target HW ID                                   | 16
 * Expansion Point No. 4                            |  0
 * Number of Target HW ID with Positions            | 16
 * % Target HW ID with Positions Length             | 16
 * % Target HW ID with Positions                    | 16
 * % Number of Target HW ID Positions               | 16
 * %& Position Length                               | 16
 * %& Position                                      | 16
 * Expansion Point No. 5                            |  0
 * Number of Data Files                             | 16
 * + Data File Pointer                              | 16
 * + Data File Name Length                          | 16
 * + Data File Name                                 | 16
 * + Data File PN Length                            | 16
 * + Data File PN                                   | 16
 * + Data File Length                               | 32
 * + Data File CRC                                  | 16
 * + Data File Length in Bytes                      | 64
 * + Data File Check Value Length                   | 16
 * + Data File Check Value Type                     | 16
 * + Data File Check Value                          | 16
 * + Expansion Point No. 6                          |  0
 * Expansion Point No. 7                            |  0
 * Number of Support Files                          | 16
 * # Support File Pointer                           | 16
 * # Support File Name Length                       | 16
 * # Support File Name                              | 16
 * # Support File PN Length                         | 16
 * # Support File PN                                | 16
 * # Support File Length                            | 32
 * # Support File CRC                               | 16
 * # Support File Check Value Length                | 16
 * # Support File Check Value Type                  | 16
 * # Support File Check Value                       | 16
 * # Expansion Point No. 8                          |  0
 * Expansion Point No. 9                            |  0
 * User Defined Data                                | 16
 * Expansion Point No. 10                           |  0
 * Load Check Value Length                          | 16
 * Load Check Value Type                            | 16
 * Load Check Value                                 | 16
 * Header File CRC                                  | 16
 * Load CRC                                         | 32
 *
 * @par Header File CRC
 * The Header File CRC is a 16-bit CRC covering fields in the Header file,
 * excluding the Header File CRC and the Load CRC field.
 **/
class ARINC_665_EXPORT LoadHeaderFile final : public Arinc665File
{
  public:
    //! Positions
    using Positions = std::list< std::string >;
    //! Target Hardware IDs / Positions
    using TargetHardwareIdsPositions = std::list< std::pair< std::string, Positions > >;
    //! Target Hardware IDs
    using TargetHardwareIds = std::list< std::string >;
    //! Load Type (Description + ID)
    using LoadType = std::optional< std::pair< std::string, uint16_t > >;

    //! Offset of the Spare Field (since ARINC 665-2)
    static constexpr auto SpareFieldOffsetV2{ std::ptrdiff_t{6} };

    //! Offset of the Part Flags Field (since ARINC 665-3) - Spare in older supplements
    static constexpr auto PartFlagsFieldOffsetV3{ std::ptrdiff_t{6} };

    //! Offset of the Load Part Number Pointer Field (since ARINC 665-1)
    static constexpr auto LoadPartNumberPointerFieldOffsetV1{ std::ptrdiff_t{6} };

    //! Offset of the Load Part Number Pointer Field (since ARINC 665-2)
    static constexpr auto LoadPartNumberPointerFieldOffsetV2{ std::ptrdiff_t{8} };

    //! Offset of the THW IDs Pointer Field (since ARINC 665-1)
    static constexpr auto ThwIdsPointerFieldOffsetV1{ std::ptrdiff_t{10} };

    //! Offset of the THW IDs Pointer Field (since ARINC 665-2)
    static constexpr auto ThwIdsPointerFieldOffsetV2{ std::ptrdiff_t{12} };

    //! Offset of the Data Files Pointer Field (since ARINC 665-1)
    static constexpr auto DataFilesPointerFieldOffsetV1{ std::ptrdiff_t{14} };

    //! Offset of the Data Files Pointer Field (since ARINC 665-2)
    static constexpr auto DataFilesPointerFieldOffsetV2{ std::ptrdiff_t{16} };

    //! Offset of the Support Files Pointer Field (since ARINC 665-1)
    static constexpr auto SupportFilesPointerFieldOffsetV1{ std::ptrdiff_t{18} };

    //! Offset of the Support Files Pointer Field (since ARINC 665-2)
    static constexpr auto SupportFilesPointerFieldOffsetV2{ std::ptrdiff_t{20} };

    //! Offset of the User Defined Data Pointer Field (since ARINC 665-1)
    static constexpr auto UserDefinedDataPointerFieldOffsetV1{ std::ptrdiff_t{22} };

    //! Offset of the User Defined Data Pointer Field (since ARINC 665-2)
    static constexpr auto UserDefinedDataPointerFieldOffsetV2{ std::ptrdiff_t{24} };

    //! Offset of the Load Type Description Pointer Field (since ARINC 665-3)
    static constexpr auto LoadTypeDescriptionPointerFieldOffsetV3{ std::ptrdiff_t{28} };

    //! Offset of the THW IDs with Positions Pointer Field (since ARINC 665-3)
    static constexpr auto ThwIdPositionsPointerFieldOffsetV3{ std::ptrdiff_t{32} };

    //! Offset of the Load Check Value Pointer Field (since ARINC 665-3)
    static constexpr auto LoadCheckValuePointerFieldOffsetV3{ std::ptrdiff_t{36} };

    //! First Start of pointer data for ARINC 665-1 Load Headers.
    static constexpr auto LoadHeaderSizeV1{ std::ptrdiff_t{26} };

    //! First Start of pointer data for ARINC 665-2 Load Headers.
    static constexpr auto LoadHeaderSizeV2{ std::ptrdiff_t{28} };

    //! First Start of pointer data for ARINC 665-3/4 Load Headers.
    static constexpr auto LoadHeaderSizeV3{ std::ptrdiff_t{40} };

    //! Position of Load CRC from the end of the Load Header File.
    static constexpr auto LoadCrcOffset{ std::ptrdiff_t{4} };

    //! Position of File CRC from the end of the Load Header File.
    static constexpr auto FileCrcOffset{ std::ptrdiff_t{6} };

    //! Download Flag of Part Flag
    static constexpr std::uint16_t PartFlagDownload{ 0x0001U };

    /**
     * @name Load CRC
     *
     * The Load CRC is a 32-bit CRC covering the entire Software Load, including all Data Files, Support Files, and
     * Header File contents excluding the "Load CRC" itself.
     * The Load CRC should be calculated in the following order:
     *  1. Header file contents excluding the `Load CRC` field,
     *  2. Data files in the sequence they are listed in the header file, and
     *  3. Support files in the sequence they are listed in the header file
     *
     * @{
     **/

    /**
     * @brief Processes the Load CRC over the given Load Header Raw representation.
     *
     * Must be used to determine the correct size of the data to be processed.
     *
     * @param[in] rawFile
     *   Load Header File Raw representation.
     * @param[in,out] loadCrc
     *   Processed CRC state.
     **/
    static void processLoadCrc( Helper::ConstRawDataSpan rawFile, Arinc649::Arinc649Crc32 &loadCrc );

    /**
     * @brief Encodes the Load CRC within the Raw Load Header File.
     *
     * @param[in,out] rawFile
     *   Raw representation of Load Header File
     * @param[in] crc
     *   Load CRC
     **/
    static void encodeLoadCrc( Helper::RawDataSpan rawFile, uint32_t crc );

    /**
     * @brief Decodes the Load CRC within the Raw Load Header File.
     *
     * @param[in] rawFile
     *   Raw representation of Load Header File
     *
     * @return Load CRC
     **/
    [[nodiscard]] static uint32_t decodeLoadCrc( Helper::ConstRawDataSpan rawFile );

    /** @} **/

    /**
     * @name Load Check Value
     *
     * The Load Check Value should be calculated in the following order:
     *  1. Header file contents excluding the `Load Check Length`, the `Load Check Value Type`, the `Load Check Value`,
     *     the `Header File CRC`, and the `Load CRC`,
     *  2. Data files in the sequence they are listed in the header file, and
     *  3. Support files in the sequence they are listed in the header file.
     *
     * @{
     **/

    /**
     * @brief Processes the Load Check Value over the given Load Header Raw representation.
     *
     * Must be used to determine the correct size of the data to be processed.
     *
     * After Completion the Load Check Value can be set via encodeLoadCheckValue() or checked via
     * @ref decodeLoadCheckValue().
     *
     * @param[in] rawFile
     *   Load Header File Raw representation.
     * @param[in,out] checkValueGenerator
     *   Processed Check Value Generator state.
     **/
    static void processLoadCheckValue(
      Helper::ConstRawDataSpan rawFile,
      Arinc649::CheckValueGenerator &checkValueGenerator );

    /**
     * @brief Encodes the Load Check Value within the Raw Load Header File.
     *
     * Also recalculates the Load Header CRC.
     *
     * @param[in,out] rawFile
     *   Raw representation of Load Header File
     * @param[in] checkValue
     *   Load Check Value
     **/
    static void encodeLoadCheckValue( Helper::RawDataSpan rawFile, const Arinc649::CheckValue &checkValue );

    /**
     * @brief Decodes the Load Check Value within the Raw Load Header File.
     *
     * @param[in] rawFile
     *   Raw representation of Load Header File
     *
     * @return Load Check Value
     * @retval Arinc649::NoCheckValue
     *   When no Load Check Value is stored.
     **/
    [[nodiscard]] static Arinc649::CheckValue decodeLoadCheckValue( Helper::ConstRawDataSpan rawFile );

    /** @} **/

    /**
     * @brief Creates an empty load header file.
     *
     * @param[in] version
     *   ARINC 665 version.
     **/
    explicit LoadHeaderFile( SupportedArinc665Version version = SupportedArinc665Version::Supplement345 );

    /**
     * @brief Creates a load header file from the given raw data.
     *
     * @param[in] rawFile
     *   Raw data file representation.
     **/
    explicit LoadHeaderFile( Helper::ConstRawDataSpan rawFile );

    //! Destructor.
    ~LoadHeaderFile() override = default;

    //! @copydoc Arinc665File::operator=
    LoadHeaderFile& operator=( Helper::ConstRawDataSpan rawFile ) override;

    //! @copydoc Arinc665File::fileType() const noexcept
    [[nodiscard]] FileType fileType() const noexcept override;

    /**
     * @name Load Part Flags
     *
     * @par Definition ARINC 665-5
     * The Part Flags are defined by a 16-bit word.
     * They are used to indicate extra information to help operators and systems distinguish and understand the purpose
     * of a part.
     *
     * @par Download Flag
     * The Least Significant Bit is used to indicate an upload/download part.
     * The value of @p 0 indicates an upload.
     * The value of @p 1 indicates that the part contains instructions for download.
     * A data loader may use this field to help the operator select parts that correspond with a download operation
     * versus an upload operation.
     *
     * @par Spare Flags
     * The other 15 bits of the 16-bit Part Flags field are reserved for future use and should be set to binary 0.
     *
     * @{
     **/

    /**
     * @brief Returns the Part Flags.
     *
     * The part flags exist since ARINC 665-3.
     * It is only encoded/ decoded within such protocol files.
     *
     * @return Part Flags.
     **/
    [[nodiscard]] uint16_t partFlags() const noexcept;

    /**
     * @brief Updates the Part Flags.
     *
     * @param[in] partFlags
     *   New Part Flags.
     **/
    void partFlags( uint16_t partFlags ) noexcept;

    /** @} **/

    /**
     * @name Load Part Number
     *
     * Each _Load Header File_ (Load) shall have a unique _Part Number_.
     * @{
     **/

    /**
     * @brief Returns the Part Number of the Load Header File.
     *
     * @return Part Number of the Load Header File.
     **/
    [[nodiscard]] std::string_view partNumber() const;

    /**
     * @brief Updates the Part Number of the Load Header File.
     *
     * @param[in] partNumber
     *   New Part Number.
     **/
    void partNumber( std::string partNumber );

    /** @} **/

    /**
     * @name Target Hardware IDs
     * @{
     **/

    /**
     * @brief Returns only the Target Hardware IDs (without position information)
     *
     * @return Target Hardware IDs.
     **/
    [[nodiscard]] const TargetHardwareIds& targetHardwareIds() const;

    //! @copydoc targetHardwareIds() const
    [[nodiscard]] TargetHardwareIds& targetHardwareIds();

    /**
     * @brief Updates Target Hardware IDs without position information.
     *
     * @param[in] targetHardwareIds
     *   Target Hardware IDs.
     **/
    void targetHardwareIds( TargetHardwareIds targetHardwareIds );

    /**
     * @brief Add Target Hardware ID.
     *
     * @param[in] targetHardwareId
     *   Target Hardware ID.
     **/
    void targetHardwareId( std::string targetHardwareId );

    /** @} **/

    /**
     * @name Target Hardware IDs with Positions
     *
     * Target HW ID with Position is not intended to replace the Target HW ID, which remains mandatory.
     * Target HW ID with Positions is only used to restrict the LSP upload into a specific position of a Target HW ID.
     *
     * @{
     **/

    /**
     * @brief Returns the Target Hardware ID/ Positions.
     *
     * @return Target Hardware IDs
     **/
    [[nodiscard]] const TargetHardwareIdsPositions& targetHardwareIdsPositions() const;

    //! @copydoc targetHardwareIdsPositions() const
    TargetHardwareIdsPositions& targetHardwareIdsPositions();

    /**
     * @brief Sets the Target Hardware ID/ Positions.
     *
     * @param[in] targetHardwareIdsPositions
     *   Target Hardware ID/ Positions.
     **/
    void targetHardwareIdsPositions( TargetHardwareIdsPositions targetHardwareIdsPositions );

    /**
     * @brief Add Target Hardware ID/ Positions.
     *
     * @param[in] targetHardwareId
     *   Target Hardware ID.
     * @param[in] positions
     *   Positions
     **/
    void targetHardwareIdPositions( std::string targetHardwareId, Positions positions );

    /** @} **/

    /**
     * @name Load Type
     * The `Load Type Description` string describes the load or the function the load performs (e.g. "EEC Operational
     * Software," "FMS Navigation Data Base", etc.).
     *
     * The `Load Type ID` is a 16-bit hexadecimal numeric value set by the manufacturer or system integrator.
     * The value in this field should correspond with the content of the `Load Type Description` field.
     * `Load Type ID` should be unique for each LSP type which is loaded to a particular target hardware type.
     *
     * Load Type ID is used to easily identify the software part type.
     * This allows the target to identify which load the incoming load replaces and where to place it in memory.
     *
     * @sa LoadType
     * @{
     **/

    /**
     * @brief Returns the Load Type Information.
     *
     * This information is only used in ARINC 665-3/4 files.
     *
     * @return Load Type information.
     **/
    [[nodiscard]] const LoadType& loadType() const;

    /**
     * @brief Updates the Load Type Information.
     *
     * This information is only used in ARINC 665-3/4 files.
     *
     * @param[in] type
     *   Load Type Information.
     **/
    void loadType( LoadType type );

    /** @} **/

    /**
     * @name Data Files
     * @sa LoadFilesInfo, LoadFileInfo
     * @{
     **/

    /**
     * @brief Return the Data Files.
     *
     * @return Data files information.
     **/
    [[nodiscard]] const LoadFilesInfo& dataFiles() const noexcept;

    //! @copydoc dataFiles() const
    LoadFilesInfo& dataFiles() noexcept;

    /**
     * @brief Updates the Data Files Information.
     *
     * @param[in] filesInformation
     *   Data Files Information.
     **/
    void dataFiles( LoadFilesInfo filesInformation );

    /**
     * @brief Add Data %File.
     *
     * @param[in] dataFileInfo
     *   Data file information.
     **/
    void dataFile( LoadFileInfo dataFileInfo );

    /** @} **/

    /**
     * @name Support Files
     * @sa LoadFilesInfo, LoadFileInfo
     * @{
     **/

    /**
     * @brief Return the Support Files.
     *
     * @return Support Files.
     **/
    [[nodiscard]] const LoadFilesInfo& supportFiles() const noexcept;

    //! @copydoc supportFiles() const
    LoadFilesInfo& supportFiles() noexcept;

    /**
     * @brief Updates the Support Files Information.
     *
     * @param[in] filesInformation
     *   Support Files Information.
     **/
    void supportFiles( LoadFilesInfo filesInformation );

    /**
     * @brief Add Support %File.
     *
     * @param[in] supportFileInfo
     *   Support File.
     **/
    void supportFile( LoadFileInfo supportFileInfo );

    /** @} **/

    /**
     * @name User Defined Data
     * @{
     **/

    /**
     * @brief Returns the User Defined Data.
     *
     * @return User defined data.
     **/
    [[nodiscard]] Helper::ConstRawDataSpan userDefinedData() const;

    /**
     * @brief Updates the User Defined Data.
     *
     * @param[in] userDefinedData
     *   User defined data.
     **/
    void userDefinedData( Helper::RawData userDefinedData );

    /** @} **/

    /**
     * @name Load Check Value Type
     *
     * Check Value Type for the whole Load.
     * This information is used to determine the size, which is reserved on raw file generation.
     *
     * @sa encodeLoadCheckValue()
     * @sa decodeLoadCheckValue()
     * @sa CheckValue
     *
     * @{
     **/

    /**
     * @brief Returns the Load Check Value Type.
     *
     * @return Load Check Value Type.
     **/
    [[nodiscard]] Arinc649::CheckValueType loadCheckValueType() const;

    /**
     * @brief Updates the Load Check Value Type.
     *
     * @param[in] type
     *   Load Check Value Type.
     **/
    void loadCheckValueType( Arinc649::CheckValueType type );

    /** @} **/

  private:
    //! @copydoc Arinc665File::encode
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the body of the load header file.
     *
     * @param[in] rawFile
     *   Raw Load Header File representation.
     **/
    void decodeBody( Helper::ConstRawDataSpan rawFile );

    /**
     * @brief Encodes the Data Files Information List.
     *
     * @param[in] encodeV3Data
     *   If set to true, additional data as stated in ARINC 665-3 is encoded.
     *
     * @return Raw representation of the files information list.
     **/
    [[nodiscard]] Helper::RawData encodeDataFiles( bool encodeV3Data ) const;

    /**
     * @brief Encodes the Support Files Information List.
     *
     * @param[in] encodeV3Data
     *   If set to true, additional data as stated in ARINC 665-3 is encoded.
     *
     * @return Raw representation of the files information list.
     **/
    [[nodiscard]] Helper::RawData encodeSupportFiles( bool encodeV3Data ) const;

    /**
     * @brief Decodes the Data Files List from the raw data.
     *
     * @param[in] rawData
     *   Raw Load Header File representation.
     * @param[in] decodeV3Data
     *   If set to true, additional data as stated in ARINC 665-3 is decoded.
     **/
    void decodeDataFiles( Helper::ConstRawDataSpan rawData, bool decodeV3Data );

    /**
     * @brief Decodes the Support Files List from the raw data.
     *
     * @param[in] rawData
     *   Raw Load Header File representation.
     * @param[in] decodeV3Data
     *   If set to true, additional data as stated in ARINC 665-3 is decoded.
     **/
    void decodeSupportFiles( Helper::ConstRawDataSpan rawData, bool decodeV3Data );

    /**
     * @brief Checks if the User Defined Data Size is a multiple of 2.
     **/
    void checkUserDefinedData();

    //! Part Flags
    uint16_t partFlagsV{ 0U };
    //! Part Number of the Load
    std::string partNumberV;
    //! List of compatible Target Hardware IDs
    TargetHardwareIds targetHardwareIdsV;
    //! List of compatible Target Hardware IDs with Positions
    TargetHardwareIdsPositions targetHardwareIdsPositionsV;
    //! Load Type
    LoadType typeV;
    //! List of Data Files
    LoadFilesInfo dataFilesV;
    //! List of Support files
    LoadFilesInfo supportFilesV;
    //! User Defined Data
    Helper::RawData userDefinedDataV;
    //! Load Check Value (Type) (since ARINC 665-3) - Value is calculated on
    //! generation.
    Arinc649::CheckValueType loadCheckValueTypeV{ Arinc649::CheckValueType::NotUsed };
};

}

#endif
