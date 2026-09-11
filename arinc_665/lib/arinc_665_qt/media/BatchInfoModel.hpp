// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::BatchInfoModel.
 **/

#ifndef ARINC_665_QT_MEDIA_BATCHINFOMODEL_HPP
#define ARINC_665_QT_MEDIA_BATCHINFOMODEL_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QAbstractTableModel>

#include <variant>

namespace Arinc665Qt::Media {

/**
 * @brief Table Model of Batch Qt Information.
 *
 * Presents the List of Batch Information as a table.
 **/
 class ARINC_665_QT_EXPORT BatchInfoModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns
    {
      //! Target Hardware ID + Position
      TargetHardwareIdPosition,

      ColumnsCount
    };

    /**
     * @brief Initialises the Batch Information Model.
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit BatchInfoModel( QObject * parent = nullptr );

    //! Destructor
    ~BatchInfoModel() override;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Index-parent - assumed to be the root element (invalid).
     *
     * @return Number of Batch Information.
     * @retval 0
     *   If the index @p parent is valid (is not the root element)
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the number of columns.
     *
     * @param[in] parent
     *   Index-parent - assumed to be the root element (invalid).
     *
     * @return Columns::ColumnsCount.
     * @retval 0
     *   If the index @p parent is valid (is not the root element)
     **/
    [[nodiscard]] int columnCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the requested data.
     *
     * @param[in] index
     *   Index of the requested item.
     * @param[in] role
     *   Requested role.
     *
     * @return Data dependent of the index and role.
     **/
    [[nodiscard]] QVariant data( const QModelIndex &index, int role ) const override;

    /**
     * @brief Returns the data for the given role and section in the header with the specified orientation.
     *
     * @param[in] section
     *   Section number
     * @param[in] orientation
     *   Orientation
     * @param[in] role
     *   Item role.
     *
     * @return Header data for the given parameters.
     **/
    [[nodiscard]] QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;

    /**
     * @name Batch Information
     * @{
     **/

    /**
     * @brief Returns the number of Batch Information.
     *
     * @return Number of Batch Information
     **/
    [[nodiscard]] size_t numberOfBatchInformation() const;

    /**
     * @brief Returns the Batches.
     *
     * @return Batches
     **/
    [[nodiscard]] const Arinc665::Media::BatchInformationVariant& batchInformation() const;

    /**
     * @brief Updates the Data Model with the given Batch Information.
     *
     * @param[in] information
     *   Batch Information, contained by the model.
     **/
    void batchInformation( Arinc665::Media::BatchInformationVariant information );

    /** @} **/

    /**
     * @brief Returns the Batch Target Information for the given index.
     *
     * @param[in] index
     *   Model Index of the requested item.
     *
     * @return Batch Target Information for the given index.
     **/
    [[nodiscard]] Arinc665::Media::BatchTargetInformationVariant batchTargetInformation(
      const QModelIndex &index ) const;

    /**
     * @brief Return Batch Target Information for a given Index.
     *
     * @param[in] index
     *   Batch Index
     *
     * @return Batch Target Information for a given Index
     * @retval {}
     *   If index is invalid
     **/
    [[nodiscard]] Arinc665::Media::BatchTargetInformationVariant batchTargetInformation( std::size_t index ) const;

    /**
     * @brief Converts a given Batch Variant to Const Batch Pointer.
     *
     * If the variant stores a const batch, it is returned directly, otherwise converts it to a const batch pointer.
     *
     * @param[in] batchTargetInformation
     *   Batch Variant
     *
     * @return Const Batch Pointer
     **/
    [[nodiscard]] Arinc665::Media::ConstBatchTargetInformation constBatchTargetInformation(
      const Arinc665::Media::BatchTargetInformationVariant &batchTargetInformation ) const;

  private:
    //! Batch Information
    Arinc665::Media::BatchInformationVariant batchInformationV;
};

}


#endif
