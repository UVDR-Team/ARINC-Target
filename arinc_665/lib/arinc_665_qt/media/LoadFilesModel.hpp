// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::LoadFilesModel.
 **/

#ifndef ARINC_665_QT_MEDIA_LOADFILESMODEL_HPP
#define ARINC_665_QT_MEDIA_LOADFILESMODEL_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QAbstractTableModel>

#include <memory>

namespace Arinc665Qt::Media {

/**
 * @brief Qt Table Model of Load Files.
 *
 * Lists the files, which are part of a load (data or support files).
 **/
class ARINC_665_QT_EXPORT LoadFilesModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns
    {
      //! Filename
      Name,
      //! Part Number
      PartNumber,

      ColumnsCount
    };

    /**
     * @brief Initialises the load files model.
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit LoadFilesModel( QObject * parent = nullptr );

    //! Destructor
    ~LoadFilesModel() override;

    /**
     * @brief Returns the Number of Rows.
     *
     * @param[in] parent
     *   Index-parent - assumed to be the root element (invalid).
     *
     * @return Number of Files.
     * @retval 0
     *   If the index @p parent is valid (is not the root element)
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the Number of Columns.
     *
     * @param[in] parent
     *   Index-parent.
     *
     * @return Columns::ColumnsCount.
     * @retval 0
     *   If index @p parent is valid (not root element)
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
    [[nodiscard]] QVariant headerData( int section, ::Qt::Orientation orientation, int role ) const override;

    /**
     * @name Load Files
     * @{
     **/

    /**
     * @brief Updates the Data Model with the given Files.
     *
     * @param[in] loadFiles
     *   Load Files, contained by the model.
     **/
    void loadFiles( Arinc665::Media::ConstLoadFiles loadFiles );

    /**
     * @brief Return Load File for the given Index.
     *
     * @param[in] index
     *   Model Index.
     *
     * @return Load File for the given Index.
     * @retval std::nullopt
     *   If the @p index is out of range or invalid.
     **/
    [[nodiscard]] std::optional< Arinc665::Media::ConstLoadFile > loadFile( const QModelIndex &index ) const;

    /**
     * @brief Return Load File for the given Index.
     *
     * @param[in] index
     *   Index of Element.
     *
     * @return Load File for the given Index.
     * @retval std::nullopt
     *   If the @p index is out of range.
     **/
    [[nodiscard]] std::optional< Arinc665::Media::ConstLoadFile > loadFile( size_t index ) const;

    /** @} **/

  private:
    //! Load Files
    Arinc665::Media::ConstLoadFiles loadFilesV;
};

}

#endif
