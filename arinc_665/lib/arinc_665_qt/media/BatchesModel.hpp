// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::BatchesModel.
 **/

#ifndef ARINC_665_QT_MEDIA_BATCHESMODEL_HPP
#define ARINC_665_QT_MEDIA_BATCHESMODEL_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QAbstractTableModel>

#include <variant>

namespace Arinc665Qt::Media {

/**
 * @brief Qt Table Model of List of Batches.
 *
 * A List of Batches is shown with batch attributes like name, part number, and comment.
 **/
class ARINC_665_QT_EXPORT BatchesModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns
    {
      //! Batch Name
      Name,
      //! Batch Part Number
      PartNumber,
      //! Comment
      Comment,

      ColumnsCount
    };

    /**
     * @brief Initialises the Batches Model.
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit BatchesModel( QObject *parent = nullptr );

    //! Destructor
    ~BatchesModel() override;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Index-parent - assumed to be the root element (invalid).
     *
     * @return Number of loads.
     * @retval 0
     *   If @p is valid (not root element)
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
     *   If @p is valid (not root element)
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
     *   Section Number
     * @param[in] orientation
     *   Orientation
     * @param[in] role
     *   Item role.
     *
     * @return Header data for the given parameters.
     **/
    [[nodiscard]] QVariant headerData(
      int section,
      Qt::Orientation orientation,
      int role = Qt::DisplayRole ) const override;

    /**
     * @name Batches
     * @{
     **/

    /**
     * @brief Returns the Number of Batches
     *
     * @return Number of Batches
     **/
    [[nodiscard]] size_t numberOfBatches() const;

    /**
     * @brief Returns the Batches.
     *
     * @return Batches
     **/
    [[nodiscard]] const Arinc665::Media::BatchesVariant& batches() const;

    /**
     * @brief Updates the Data Model with the given Batches.
     *
     * @param[in] batches
     *   Batches, contained by the model.
     **/
    void batches( Arinc665::Media::BatchesVariant batches );

    /** @} **/

    /**
     * @brief Returns the Batch for the given index.
     *
     * @param[in] index
     *   Index of the requested item.
     *
     * @return Batch for the given index.
     **/
    [[nodiscard]] Arinc665::Media::BatchVariant batch( const QModelIndex &index ) const;

    /**
     * @brief Return Batch for a given Index.
     *
     * @param[in] index
     *   Batch Index
     *
     * @return Batch for a given Index
     * @retval {}
     *   If index is invalid
     **/
    [[nodiscard]] Arinc665::Media::BatchVariant batch( std::size_t index ) const;

    /**
     * @brief Converts a given Batch Variant to Const Batch Pointer.
     *
     * If the variant stores a const batch, it is returned directly, otherwise converts it to a const batch pointer.
     *
     * @param[in] batch
     *   Batch Variant
     *
     * @return Const Batch Pointer
     **/
    [[nodiscard]] Arinc665::Media::ConstBatchPtr constBatch( const Arinc665::Media::BatchVariant &batch ) const;

  private:
    //! Batches List
    Arinc665::Media::BatchesVariant batchesV;
};

}

#endif
