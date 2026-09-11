// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::TargetHardwareIdsPositionsModel.
 **/

#ifndef ARINC_665_QT_MEDIA_TARGETHARDWAREIDSPOSITIONSMODEL_HPP
#define ARINC_665_QT_MEDIA_TARGETHARDWAREIDSPOSITIONSMODEL_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Load.hpp>

#include <QAbstractItemModel>

namespace Arinc665Qt::Media {

/**
 * @brief Target Hardware IDs / Positions Tree Model.
 *
 * Target Hardware IDs and Positions are displayed in a tree view.
 **/
class ARINC_665_QT_EXPORT TargetHardwareIdsPositionsModel final : public QAbstractItemModel
{
    Q_OBJECT

  public:
    //! Displayed Columns
    enum class Columns
    {
      //! Target Hardware ID/ Position Information
      ThwIdPosition,

      Last
    };

    /**
     * @brief Initialises the model instance.
     *
     * @param[in] parent
     *   Parent object
     **/
    explicit TargetHardwareIdsPositionsModel( QObject * parent = nullptr );

    //! Destructor
    ~TargetHardwareIdsPositionsModel() override = default;

    /**
     * @brief Creates the index for the child identified by its parent and its row and column.
     *
     * @param[in] row
     *   Index Row
     * @param[in] column
     *   Index Column
     * @param[in] parent
     *   Parent Model Index
     *
     * @return Model Index
     **/
    [[nodiscard]] QModelIndex index( int row, int column, const QModelIndex &parent ) const override;

    /**
     * @brief Return the parent of the given index.
     *
     * @param[in] index
     *   Model Index
     *
     * @return Model Index of Parent.
     **/
    [[nodiscard]] QModelIndex parent( const QModelIndex &index ) const override;

    /**
     * @brief Return if the given parent has children.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return If the index @p parent has children.
     **/
    [[nodiscard]] bool hasChildren( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of Rows.
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the number of columns.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of Columns.
     **/
    [[nodiscard]] int columnCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the data for the given index.
     *
     * @param[in] index
     *   Model Index
     * @param[in] role
     *   Data Role
     *
     * @return Data corresponding to @p index and @p role.
     **/
    [[nodiscard]] QVariant data( const QModelIndex &index, int role ) const override;

    /**
     * @brief Returns the header data.
     *
     * @param[in] section
     *   Section
     * @param[in] orientation
     *   Orientation
     * @param[in] role
     *   Display role.
     *
     * @return Corresponding Header Data.
     **/
    [[nodiscard]] QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;

    /**
     * @brief Assigns the THW IDs and Positions Information.
     *
     * @param[in] thwIdsPositions
     *   THW IDs and Positions.
     **/
    void targetHardwareIdsPositions( Arinc665::Media::Load::TargetHardwareIdPositions thwIdsPositions );

  private:
    /**
     * @brief Returns that for Target Hardware ID.
     *
     * @param[in] index
     *   Model Index
     * @param[in] role
     *   Role to display
     *
     * @return Data Representation.
     **/
    [[nodiscard]] QVariant dataThwId( const QModelIndex &index, int role ) const;

    /**
     * @brief Returns that for Target Position.
     *
     * @param[in] index
     *   Model Index
     * @param[in] role
     *   Role to display
     *
     * @return Data Representation.
     **/
    [[nodiscard]] QVariant dataPosition( const QModelIndex &index, int role ) const;

    //! THW ID Position Element
    Arinc665::Media::Load::TargetHardwareIdPositions thwIdsPositionsV;
};

}

#endif
