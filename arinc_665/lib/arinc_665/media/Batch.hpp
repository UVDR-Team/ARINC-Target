// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Media::Batch
 **/

#ifndef ARINC_665_MEDIA_BATCH_HPP
#define ARINC_665_MEDIA_BATCH_HPP

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/File.hpp>

#include <map>
#include <string>
#include <string_view>

namespace Arinc665::Media {

/**
 * @brief %Batch of %Loads.
 *
 * A %Batch is used to declare loads for multiple Target Hardware Items at a time.
 * For each Target Hardware (identified by its Target Hardware ID - THW ID) a list of loads is defined.
 **/
class ARINC_665_EXPORT Batch final : public File
{
  public:
    /**
     * @brief Initialises the batch with the given data.
     *
     * An instance cannot be created directly.
     * Refer to ContainerEntity::addBatch() for instantiation.
     *
     * @param[in] parent
     *   Parent of this file.
     * @param[in] name
     *   Name of the %Batch.
     * @param[in] mediumNumber
     *   Medium Number, where the file is located on.
     * @param[in] createKey
     *   Key to protect creation.
     **/
    Batch(
      const ContainerEntityPtr &parent,
      std::string name,
      const OptionalMediumNumber &mediumNumber,
      const CreateKey &createKey );

    /**
     * @copydoc File::fileType()
     *
     * @retval FileType::BatchFile
     *   Always
     **/
    [[nodiscard]] FileType fileType() const override;

    /**
     * @name Batch Part Number
     * A batch has its own Part Number.
     *
     * @{
     **/

    /**
     * @brief Return the Part Number of the %Batch.
     *
     * @return Part Number of the %Batch.
     **/
    [[nodiscard]] std::string_view partNumber() const;

    /**
     * @brief Updates the %Batch Part Number.
     *
     * @param[in] partNumber
     *   New %Batch Part Number
     **/
    void partNumber( std::string partNumber );

    /** @} **/

    /**
     * @name %Batch Comment
     *
     * A Batch can have an addition comment.
     * This field may be used to include the batch definition design information or modification history of the %Batch
     * %File.
     *
     * @{
     **/

    /**
     * @brief Get the comment, which describes the batch.
     *
     * @return Comment, which describes the batch.
     **/
    [[nodiscard]] std::string_view comment() const;

    /**
     * @brief Set the comment, which describes the batch.
     *
     * @param[in] comment
     *   Comment, which describes the batch.
     **/
    void comment( std::string comment );

    /** @} **/

    /**
     * @name Batch Targets Information
     *
     * Defines the Target HW ID POS and Loads information, which are loaded by this batch.
     *
     * @{
     **/

    /**
     * @brief Return %Batch Information for Targets.
     *
     * @return %Batch Target Information.
     **/
    [[nodiscard]] ConstBatchInformation targets() const;

    /**
     * @brief Return the Loads for the given Target Hardware ID Position.
     *
     * @param[in] targetHardwareIdPosition
     *   Target Hardware ID Position
     *
     * @return Loads corresponding to @p targetHardwareIdPosition.
     **/
    [[nodiscard]] ConstLoads target( std::string_view targetHardwareIdPosition ) const;

    /**
     * @brief Add Loads for the given Target Hardware ID Position.
     *
     * @note
     * As loads are stored as weak references, the parameters are not moved.
     *
     * @param[in] targetHardwareIdPosition
     *   Target Hardware ID Position
     * @param[in] loads
     *   Loads for @p targetHardwareIdPosition.
     **/
    void target( std::string targetHardwareIdPosition, const ConstLoads &loads );

    /**
     * @brief Add the given Load to the Target Hardware ID Position.
     *
     * @note
     * As loads are stored as weak references, the parameters are not moved.
     *
     * @param[in] targetHardwareIdPosition
     *   Target Hardware ID Position
     * @param[in] load
     *   Load to add.
     **/
    void target( std::string_view targetHardwareIdPosition, const ConstLoadPtr &load );

    /** @} **/

  private:
    //! Weak %Loads List
    using WeakLoads = std::list< ConstLoadPtr::weak_type >;

    //! Batch Information (Target Hardware ID -> Weak Loads)
    using WeakBatchInfo = std::map< std::string, WeakLoads, std::less<> >;

    //! Part Number
    std::string partNumberV;
    //! Batch Comment
    std::string commentV;
    //! Batch Information
    WeakBatchInfo batchesV;
};

}

#endif
