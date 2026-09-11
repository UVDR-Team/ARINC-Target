// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::LoadsModel.
 **/

#ifndef ARINC_665_QT_MEDIA_LOADSMODEL_HPP
#define ARINC_665_QT_MEDIA_LOADSMODEL_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QAbstractTableModel>

namespace Arinc665Qt::Media {

/**
 * @brief Qt Table Model of List of Loads.
 *
 * @sa @ref Arinc665::Media::Load
 **/
class ARINC_665_QT_EXPORT LoadsModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns
    {
      //! Load Filename
      Name,
      //! Load Part Number
      PartNumber,
      //! Load Type
      LoadType,

      ColumnsCount
    };

    /**
     * @brief Initialises the loads model.
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit LoadsModel( QObject * parent = nullptr );

    //! Destructor
    ~LoadsModel() override;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Index-parent - assumed to be the root element (invalid).
     *
     * @return Number of loads.
     * @retval 0
     *   If the index @p parent is valid (not the root element)
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the number of columns.
     *
     * @param[in] parent
     *   Index-parent - assumed to be the root element (invalid).
     *
     * @retval 0
     *   If the index @p parent is valid (not root element)
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
     * @name Loads
     * @{
     **/

    /**
     * @brief Returns the Number of Loads
     *
     * @return Number of Loads
     **/
    [[nodiscard]] size_t numberOfLoads() const;

    /**
     * @brief Returns the Loads.
     *
     * @return Loads
     **/
    [[nodiscard]] const Arinc665::Media::LoadsVariant& loads() const;

    /**
     * @brief Updates the data model with the given ASF messages.
     *
     * @param[in] loads
     *   Loads, contained by the model.
     **/
    void loads( Arinc665::Media::LoadsVariant loads );


    /**
     * @brief Converts a given Loads Variant to a Const Loads List.
     *
     * If the variant stores a const load list, it is returned directly, otherwise converts it to a const load list.
     *
     * @param[in] loads
     *   Loads Variant
     *
     * @return Const Loads List
     **/
    [[nodiscard]] Arinc665::Media::ConstLoads constLoads( const Arinc665::Media::LoadsVariant &loads ) const;

    /**
     * @brief Returns the load for the given index.
     *
     * @param[in] index
     *   Index of the requested item.
     *
     * @return Load for the given index.
     * @retval {}
     *   If @p index is invalid
     **/
    [[nodiscard]] Arinc665::Media::LoadVariant load( const QModelIndex &index ) const;

    /**
     * @brief Return Load for a given Index.
     *
     * @param[in] index
     *   Load Index
     *
     * @return Load for a given Index
     * @retval {}
     *   If @p index is invalid
     **/
    [[nodiscard]] Arinc665::Media::LoadVariant load( std::size_t index ) const;

    /**
     * @brief Converts a given Load Variant to a Const Load Pointer.
     *
     * If the variant stores a const load, it is returned directly, otherwise converts it to a const load pointer.
     *
     * @param[in] load
     *   Load Variant
     *
     * @return Const Load Pointer
     **/
    [[nodiscard]] Arinc665::Media::ConstLoadPtr constLoad( const Arinc665::Media::LoadVariant &load ) const;

    /** @} **/

  private:
    //! Loads List
    Arinc665::Media::LoadsVariant loadsV;
};

}

#endif
