// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Media::Load.
 **/

#ifndef ARINC_665_MEDIA_LOAD_HPP
#define ARINC_665_MEDIA_LOAD_HPP

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/File.hpp>

#include <arinc_649/CheckValue.hpp>

#include <helper/RawData.hpp>

#include <cstdint>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace Arinc665::Media {

/**
 * @brief %Load within %Media Set.
 **/
class ARINC_665_EXPORT Load final : public File
{
  public:
    //! Positions List
    using Positions = std::set< std::string, std::less<> >;
    //! Target Hardware ID / Positions
    using TargetHardwareIdPositions = std::map< std::string, Positions, std::less<> >;
    //! Target Hardware ID List
    using TargetHardwareIds = std::set< std::string, std::less<> >;
    //! %Load Type (Description + ID)
    using Type = std::optional< std::pair< std::string, uint16_t > >;

    /**
     * @brief Initialises a ARINC 665 Load.
     *
     * An instance cannot be created directly.
     * Refer to ContainerEntity::addLoad() for instantiation.
     *
     * @param[in] parent
     *   Parent of this Load.
     * @param[in] name
     *   Name of the Load
     * @param[in] mediumNumber
     *   Medium Number, where the file is located on.
     * @param[in] createKey
     *   Key to protect creation.
     **/
    Load(
      const ContainerEntityPtr &parent,
      std::string name,
      const OptionalMediumNumber &mediumNumber,
      const CreateKey &createKey );

    /**
     * @copydoc File::fileType()
     *
     * @retval FileType::LoadFile
     *   Always
     **/
    [[nodiscard]] FileType fileType() const override;

    /**
     * @name Part Flags.
     *
     * The Part Flags are defined by a 16-bit word.
     * They are used to indicate extra information to help operators and systems distinguish and understand the purpose
     * of a part.
     *
     * @par Download Flag
     * The Least Significant Bit is used to indicate an upload/download part.
     * The value of `0` indicates an upload.
     * The value of `1` indicates that the part contains instructions for download.
     * A data loader may use this field to help the operator select parts that correspond with a download operation
     * versus an upload operation.
     *
     * @note
     * In an upload operation, a data loader uses information from the LSP to cause a transfer of specific information
     * from the LSP to the target.
     * In a download operation, a data loader uses information from the LSP to cause a transfer of specific information
     * from the target to the data loader.
     *
     * The purpose of assigning a PN to a download LSP is to identify, control, and provide integrity for the files in
     * the LSP which describe to the data loader how to perform the specific download operation.
     * The PN of the download LSP only pertains to the files that are provided to the data loader which cause the
     * download.
     * The download PN does not provide configuration data for the data downloaded from the target.
     *
     * @par Spare Flags
     * The other 15 bits of the 16-bit Part Flags field are reserved for future use and should be set to binary `0`.
     *
     * @{
     **/

    /**
     * @brief Obtains the Part Flags Field.
     *
     * @return Part Flags Field.
     **/
    [[nodiscard]] uint16_t partFlags() const;

    /**
     * @brief Updates the Part Flags Field
     *
     * @param[in] partFlags
     *   Part Flags
     **/
    void partFlags( uint16_t partFlags );

    /** @} **/

    /**
     * @name Load Part Number
     * @{
     **/

    /**
     * @brief Return the Part Number of the %Load.
     *
     * @return Part Number of the %Load.
     **/
    [[nodiscard]] std::string_view partNumber() const;

    /**
     * @brief Updates the Part Number of the %Load.
     *
     * @param[in] partNumber
     *   New %Load Part Number
     **/
    void partNumber( std::string partNumber );

    /** @} **/

    /**
     * @name Target Hardware IDs with Positions.
     * @{
     **/

    /**
     * @brief Returns the List of Target HW IDs.
     *
     * @return List of Target HW IDs.
     **/
    [[nodiscard]] const TargetHardwareIdPositions&
    targetHardwareIdPositions() const;

    //! @copydoc targetHardwareIdPositions() const
    TargetHardwareIdPositions& targetHardwareIdPositions();

    /**
     * @brief Updates the Target Hardware ID/ Position.
     *
     * @param[in] targetHardwareIdPositions
     *   Target Hardware ID/ Positions.
     **/
    void targetHardwareIdPositions( TargetHardwareIdPositions targetHardwareIdPositions );

    /** @} **/

    /**
     * @name Target Hardware IDs without Positions Information.
     * @{
     **/

    /**
     * @brief Returns the List of Target HW IDs without position information.
     *
     * @return Target HW IDs.
     **/
    [[nodiscard]] TargetHardwareIds targetHardwareIds() const;

    /**
     * @brief Adds a list of Target HW IDs without position information.
     *
     * @param[in] thwIds
     *   Target HW IDs.
     **/
    void targetHardwareIds( const TargetHardwareIds &thwIds );

    /**
     * @brief Adds a Target Hardware ID to the List of THW IDs.
     *
     * @param[in] targetHardwareId
     *   New Target Hardware ID.
     * @param[in] positions
     *   Optional additional Position specification.
     **/
    void targetHardwareId( std::string targetHardwareId, Positions positions = {} );

    /** @} **/

    /**
     * @name Files Lookup
     *
     * @{
     **/

    /**
     * @brief Returns file with given filename.
     *
     * It also verifies the filename against the load header file itself.
     *
     * If more than one file is found with the given attributes no file is returned.
     *
     * @param[in] filename
     *   Filename of file to look for.
     *
     * @return %File with filename.
     * @retval {}
     *   No file or more than on file found within the load.
     **/
    [[nodiscard]] ConstFilePtr file( std::string_view filename ) const;

    /**
     * @brief Returns file with given filename and Check Value.
     *
     * It also verifies the filename against the load header file itself.
     *
     * If more than one file is found with the given attributes, no file is returned.
     *
     * If @p checkValue is set to @p Arinc649::CheckValue::NoCheckValue, only for filename is searched.
     *
     * @param[in] checkValues
     *   List of check values.
     * @param[in] filename
     *   Filename of the file to look for.
     * @param[in] checkValue
     *   Check Value of the file.
     *
     * @return %File with filename and part number if provided.
     * @retval {}
     *   No file or more than on file found within the load.
     **/
    [[nodiscard]] ConstFilePtr file(
      const CheckValues &checkValues,
      std::string_view filename,
      const Arinc649::CheckValue &checkValue ) const;

    /**
     * @brief Return all files.
     *
     * Contains:
     *  - the %Load (Header) %File itself,
     *  - all Data %Files, and
     *  - all Support %Files.
     *
     * @return All files relating to load.
     **/
    [[nodiscard]] ConstFiles files() const;

    /** @} **/

    /**
     * @name Data Files
     * A (data) file is a tuple of filename, part number, and check value type.
     *
     * @{
     **/

    /**
     * @brief Returns the data files.
     *
     * @param[in] effective
     *   Defines how to handle the check value of the files' information.
     *   If set to true, the effective check value type is returned for the file.
     *
     * @return Data files.
     **/
    [[nodiscard]] ConstLoadFiles dataFiles( bool effective = false ) const;

    /**
     * @brief Sets Data %Files.
     *
     * Replaces all data files by @p files.
     *
     * @param[in] files
     *   %Files to set.
     **/
    void dataFiles( const ConstLoadFiles &files );

    /**
     * @brief Add the given file as a _Data File_.
     *
     * @param[in] file
     *   Data %File.
     * @param[in] partNumber
     *   Part Number.
     * @param[in] checkValueType
     *   Check Value Type used for this file entry.
     *   If set to `{}` the inherited value will be used.
     *   Set to Arinc649::CheckValueType::NotUsed for not providing check value.
     *
     * @sa dataFilesCheckValueType()
     **/
    void dataFile(
      const ConstRegularFilePtr &file,
      std::string partNumber,
      std::optional< Arinc649::CheckValueType > checkValueType = {} );

    /** @} **/

    /**
     * @name Support Files
     * A (support) file is a tuple of filename, part number, and check value type.
     *
     * @{
     **/

    /**
     * @brief Returns the Support %Files.
     *
     * @param[in] effective
     *   Defines how to handle the check value of the files' information.
     *   If set to `true`, the effective check value type is returned for the file.
     *
     * @return Support %Files.
     **/
    [[nodiscard]] ConstLoadFiles supportFiles( bool effective = false ) const;

    /**
     * @brief Sets Support %Files.
     *
     * Replaces all support files by @p files.
     *
     * @param[in] files
     *   %Files to set.
     **/
    void supportFiles( const ConstLoadFiles &files );

    /**
     * @brief Add the given file as a _Support File_.
     *
     * @param[in] file
     *   Support %File.
     * @param[in] partNumber
     *   Part Number.
     * @param[in] checkValueType
     *   The Check Value Type used for this file entry.
     *   If set to `{}`, the inherited value will be used.
     *   Set to Arinc649::CheckValueType::NotUsed for not providing check value.
     *
     * @sa supportFilesCheckValueType()
     **/
    void supportFile(
      const ConstRegularFilePtr &file,
      std::string partNumber,
      std::optional< Arinc649::CheckValueType > checkValueType = {} );

    /** @} **/

    /**
     * @name User-Defined Data
     *
     * This library handles *user-defined data* as plain data without any data format.
     *
     * @{
     **/

    /**
     * @brief Returns the user-defined data stored in the load header.
     *
     * @return User-defined data.
     **/
    [[nodiscard]] Helper::ConstRawDataSpan userDefinedData() const;

    //! @copydoc Load::userDefinedData() const
    [[nodiscard]] Helper::RawData& userDefinedData();

    /**
     * @brief Sets the User-Defined data stored in the load header.
     *
     * @param[in] userDefinedData
     *   Updated user-defined data.
     **/
    void userDefinedData( Helper::RawData userDefinedData );

    /** @} **/

    /**
     * @name Load Type
     *
     * The load type is a description and an @p uint16_t ID assigned.
     *
     * @{
     **/

    /**
     * @brief Returns the %Load Type.
     *
     * @return %Load Type.
     **/
    [[nodiscard]] const Type& loadType() const;

    /**
     * @brief Updates the %Load Type.
     *
     * @param[in] type
     *   New load type.
     **/
    void loadType( Type type );

    /** @} **/

    /**
     * @name Load Check Value Type
     *
     * This information is used to determine the Check Value Type used for generation of *%Load Check Value*.
     * If not provided, the %Media Set Check Value is used.
     *
     * @{
     **/

    /**
     * @brief Returns the effective %Load Check Value Type.
     *
     * If no check value is given for %Load Check Value Type, the %Media Set Check Value Type is used.
     *
     * @return Effective %Load Check Value Type
     *
     * @sa MediaSet::mediaSetCheckValueType
     **/
    [[nodiscard]] Arinc649::CheckValueType effectiveLoadCheckValueType() const;

    /**
     * @brief Returns the %Load Check Value Type.
     *
     * @return %Load Check Value Type
     **/
    [[nodiscard]] std::optional< Arinc649::CheckValueType > loadCheckValueType() const;

    /**
     * @brief Updates the %Load Check Value Type
     *
     * @param[in] checkValueType
     *   New %Load Check Value Type.
     **/
    void loadCheckValueType( std::optional< Arinc649::CheckValueType > checkValueType );

    /** @} **/

    /**
     * @name Data %Files Check Value Type
     *
     * This information is used to determine the Check Value Type used for generation of Data %Files Check Value.
     * If not provided, the *%Media Set Check Value* is used.
     *
     * @{
     **/

    /**
     * @brief Returns the effective Data %Files Check Value Type.
     *
     * If no check value is given for Data %Files Check Value Type, the %Media Set Check Value Type is used.
     *
     * @return Effective Data %Files Check Value Type
     *
     * @sa MediaSet::mediaSetCheckValueType
     **/
    [[nodiscard]] Arinc649::CheckValueType effectiveDataFilesCheckValueType() const;

    /**
     * @brief Returns the Data %Files Check Value Type.
     *
     * @return Data %Files Check Value Type
     **/
    [[nodiscard]] std::optional< Arinc649::CheckValueType > dataFilesCheckValueType() const;

    /**
     * @brief Updates the Data %Files Check Value Type.
     *
     * @param[in] checkValueType
     *   New Data %Files Check Value Type.
     **/
    void dataFilesCheckValueType( std::optional< Arinc649::CheckValueType > checkValueType );

    /** @} **/

    /**
     * @name Support %Files Check Value Type
     *
     * This information is used to determine the Check Value Type used for generation of *Support %Files Check Value*.
     * If not provided, the *%Media Set Check Value* is used.
     *
     * @{
     **/

    /**
     * @brief Returns the effective Support %Files Check Value Type.
     *
     * If no check value is given for Support %Files Check Value Type, the %Media Set Check Value Type is used.
     *
     * @return Effective Support %Files Check Value Type
     *
     * @sa MediaSet::mediaSetCheckValueType
     **/
    [[nodiscard]] Arinc649::CheckValueType effectiveSupportFilesCheckValueType() const;

    /**
     * @brief Returns the Support %Files Check Value Type.
     *
     * @return Support %Files Check Value Type
     **/
    [[nodiscard]] std::optional< Arinc649::CheckValueType > supportFilesCheckValueType() const;

    /**
     * @brief Updates the Support %Files Check Value Type.
     *
     * @param[in] checkValueType
     *   New Support %Files Check Value Type.
     **/
    void supportFilesCheckValueType( std::optional< Arinc649::CheckValueType > checkValueType );

    /** @} **/

  private:
    //! Weak %Load %File ( file, Part Number, Check Value Type ).
    using WeakLoadFile = std::tuple<
      ConstRegularFilePtr::weak_type,
      std::string,
      std::optional< Arinc649::CheckValueType > >;
    //! Weak %Load %File List.
    using WeakLoadFiles = std::list< WeakLoadFile >;

    //! Part Flags
    uint16_t partFlagsV{};
    //! Part Number
    std::string partNumberV;
    //! Target Hardware ID/ Positions
    TargetHardwareIdPositions targetHardwareIdPositionsV;
    //! Data Files
    WeakLoadFiles dataFilesV;
    //! Support Files
    WeakLoadFiles supportFilesV;
    //! User Defined Data
    Helper::RawData userDefinedDataV;
    //! Load Type
    Type typeV;
    //! Load Check Value Type
    std::optional< Arinc649::CheckValueType > loadCheckValueTypeV;
    //! Data Files Default Check Value Type
    std::optional< Arinc649::CheckValueType > dataFilesCheckValueTypeV;
    //! Support Files Default Check Value Type
    std::optional< Arinc649::CheckValueType > supportFilesCheckValueTypeV;
};

/**
 * @name Loads lookup
 *
 * Lookup multiple loads.
 *
 * @{
 **/

/**
 * @brief Returns a Load identified by its Part Number.
 *
 * Iterates over @p loads returning the first load, whom *Part Number* matches @p partNumber.
 *
 * @param[in] loads
 *   List of %Loads.
 * @param[in] partNumber
 *   %Load Part Number
 *
 * @return %Load with the given part number.
 * @retval {}
 *   When no load with the given part number exists in @p loads
 **/
[[nodiscard]] ARINC_665_EXPORT ConstLoadPtr Loads_loadByPartNumber(
  const ConstLoads &loads,
  std::string_view partNumber );

/**
 * @brief Returns the file with the given filename from the load with the given Part Number.
 *
 * First, if @p loadPartNumber is valid (not empty), find the matching load.
 * If no load can be found, this is handled as an error.
 * Then the load is queried for the file.
 *
 * Otherwise, this operation iterates over @p loads and finds files matching the parameter @p filename.
 * If more than one file is found with the given attributes, no file is returned.
 *
 * @sa @ref Loads_loadByPartNumber()
 * @sa @ref Load::file(std::string_view) const
 *
 * @param[in] loads
 *   List of loads
 * @param[in] filename
 *   Filename
 * @param[in] loadPartNumber
 *   %Load Part Number
 *
 * @return %File with filename.
 * @retval {}
 *   No file or more than on file found within loads.
 **/
[[nodiscard]] ARINC_665_EXPORT ConstFilePtr Loads_file(
  const ConstLoads &loads,
  std::string_view filename,
  std::string_view loadPartNumber = {} );

/**
 * @brief Returns the file with the given Filename, Part Number (of the containing load), and Check Value.
 *
 * First, if @p loadPartNumber is valid (not empty), find the matching load.
 * If no load can be found, this is handled as an error.
 * Then the load is queried for the file.
 *
 * Otherwise, this operation iterates over @p loads and finds files matching parameters @p filename and @p checkValue.
 * If more than one file is found with the given attributes, no file is returned.
 *
 * @sa @ref Loads_loadByPartNumber()
 * @sa @ref Load::file(const CheckValues&,std::string_view,const Arinc649::CheckValue &) const
 *
 * @param[in] loads
 *   List of loads
 * @param[in] checkValues
 *   Check Values
 * @param[in] filename
 *   Filename
 * @param[in] loadPartNumber
 *   %Load Part Number
 * @param[in] checkValue
 *   Check Value of the file.
 *
 * @return %File with filename and part number if provided.
 * @retval {}
 *   No file or more than on file found within loads.
 **/
[[nodiscard]] ARINC_665_EXPORT ConstFilePtr Loads_file(
  const ConstLoads &loads,
  const CheckValues &checkValues,
  std::string_view filename,
  std::string_view loadPartNumber = {},
  const Arinc649::CheckValue &checkValue = Arinc649::CheckValue::NoCheckValue );

/** @} **/

}

#endif
