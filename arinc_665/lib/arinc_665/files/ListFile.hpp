// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::ListFile.
 **/


#ifndef ARINC_665_FILES_LISTFILE_HPP
#define ARINC_665_FILES_LISTFILE_HPP

#include <arinc_665/files/Files.hpp>
#include <arinc_665/files/Arinc665File.hpp>

#include <arinc_665/MediumNumber.hpp>

namespace Arinc665::Files {

/**
 * @brief Base Class for ARINC 665 Protocol List %Files.
 *
 * This intermediate class is defined to add common methods for all List Files.
 * Provides the common attributes:
 * - Media Set Part Number,
 * - Media Sequence Number, and
 * - Number of Media Set Members.
 *
 * Children are:
 * - @ref FileListFile
 * - @ref LoadListFile
 * - @ref BatchListFile
 **/
class ARINC_665_EXPORT ListFile : public Arinc665File
{
  public:
    //! Destructor.
    ~ListFile() override = default;

    /**
     * @name Media Set Part Number
     * @{
     **/

    /**
     * @brief Returns the Media Set Part Number, where the files belong to.
     *
     * @return Media Set Part Number.
     **/
    [[nodiscard]] std::string_view mediaSetPn() const;

    /**
     * @brief Sets the Media Part Number, where the files belong to.
     *
     * @param[in] mediaSetPn
     *   Media Set Part Number.
     **/
    void mediaSetPn( std::string mediaSetPn );

    /** @} **/

    /**
     * @name Media Sequence Number
     * @{
     **/

    /**
     * @brief Returns the Media Sequence Number.
     *
     * @return Media Sequence Number
     **/
    [[nodiscard]] MediumNumber mediaSequenceNumber() const;

    /**
     * @brief Updates the Media Sequence Number.
     *
     * @param[in] mediaSequenceNumber
     *   New Media Sequence Number
     **/
    void mediaSequenceNumber( MediumNumber mediaSequenceNumber );

    /** @} **/

    /**
     * @name Number of Media Set Members
     * @{
     **/

    /**
     * @brief Returns the number of Media Set members.
     *
     * @return Number of Media Set members.
     **/
    [[nodiscard]] MediumNumber numberOfMediaSetMembers() const;

    /**
     * @brief Updates the number of Media Set members.
     *
     * @param[in] numberOfMediaSetMembers
     *   New number of Media Set members
     **/
    void numberOfMediaSetMembers( MediumNumber numberOfMediaSetMembers );

    /** @} **/

    /**
     * @brief Returns the Media Set Information.
     *
     * @return Media Set Information
     **/
    [[nodiscard]] MediaSetInformation mediaSetInformation() const;

  protected:
    /**
     * @copydoc Arinc665File::Arinc665File(SupportedArinc665Version,ptrdiff_t)
     **/
    explicit ListFile(
      SupportedArinc665Version version = SupportedArinc665Version::Supplement345,
      ptrdiff_t checksumPosition = DefaultChecksumPosition ) noexcept;

    //! @copydoc Arinc665File::Arinc665File(Helper::ConstRawDataSpan,FileType,ptrdiff_t)
    ListFile(
      Helper::ConstRawDataSpan rawFile,
      FileType expectedFileType,
      ptrdiff_t checksumPosition = DefaultChecksumPosition );

    /**
     * @brief Copy Constructor
     *
     * @param[in] other
     *   Other List File
     **/
    ListFile( const ListFile &other ) = default;

    //! @copydoc ListFile(const ListFile&)
    ListFile( ListFile &&other ) = default;

    using Arinc665File::operator=;

    /**
     * @brief Assignment operator
     *
     * @param[in] other
     *   Other List File.
     *
     * @return *this
     **/
    ListFile& operator=( const ListFile &other ) = default;

    //! @copydoc operator=(const Arinc665File&)
    ListFile& operator=( ListFile &&other ) = default;

    /**
     * @brief Encodes the Media Information.
     *
     * @return Raw representation of Media Information.
     **/
    [[nodiscard]] Helper::RawData encodeMediaInformation() const;

    /**
     * @brief Decodes the Media Information.
     *
     * @param[in] rawData
     *   Raw Data
     **/
    void decodeMediaInformation( Helper::ConstRawDataSpan rawData );

  private:
    //! Media Set Part Number.
    std::string mediaSetPnV;
    //! Media Sequence Number.
    MediumNumber mediaSequenceNumberV{ 0U };
    //! Number of Media Set Members.
    MediumNumber numberOfMediaSetMembersV{ 0U };
};

}

#endif
