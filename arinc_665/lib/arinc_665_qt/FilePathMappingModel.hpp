// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::FilePathMappingModel.
 **/

#ifndef ARINC_665_QT_FILEPATHMAPPINGMODEL_HPP
#define ARINC_665_QT_FILEPATHMAPPINGMODEL_HPP

#include <arinc_665_qt/Arinc665Qt.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QAbstractTableModel>

namespace Arinc665Qt {

/**
 * @brief File Path Mapping Qt Model.
 *
 * This model provides a table of Media Set Files and their corresponding file system path.
 **/
class ARINC_665_QT_EXPORT FilePathMappingModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Media Set File
      MediaSetFile,
      //! File Path
      FilePath,

      ColumnsCount
    };

    /**
     * @brief Initialises the Model.
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit FilePathMappingModel( QObject *parent = nullptr );

    //! Destructor
    ~FilePathMappingModel() override;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Index-parent - assumed to be the root element (invalid).
     *
     * @return Number of Media.
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
     * @brief Returns the data for the given role and section in the header with
     *   the specified orientation.
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
      ::Qt::Orientation orientation,
      int role = Qt::DisplayRole ) const override;

    /**
     * @brief Return all media paths.
     *
     * @return Media Paths.
     **/
    [[nodiscard]] const Arinc665::Utils::FilePathMapping &filePathMapping() const;

    /**
     * @brief Set File Path Mapping
     *
     * @param[in] filePathMapping
     *   File Path Mapping
     **/
    void filePathMapping( Arinc665::Utils::FilePathMapping filePathMapping );

  private:
    //! File Path Mapping
    Arinc665::Utils::FilePathMapping filePathMappingV;
};

}

#endif
