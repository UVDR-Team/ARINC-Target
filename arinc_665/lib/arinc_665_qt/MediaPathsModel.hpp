// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::MediaPathsModel.
 **/

#ifndef ARINC_665_QT_MEDIAPATHSMODEL_HPP
#define ARINC_665_QT_MEDIAPATHSMODEL_HPP

#include <arinc_665_qt/Arinc665Qt.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <arinc_665/MediumNumber.hpp>

#include <QAbstractTableModel>

namespace Arinc665Qt {

/**
 * @brief Qt Media Paths Model.
 *
 * This model is used to manage a list of @ref Arinc665::Utils::MediaPaths.
 *
 * @sa Arinc665::Utils::MediaPaths
 **/
class ARINC_665_QT_EXPORT MediaPathsModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Medium Number
      MediumNumber,
      //! Medium Path
      MediumPath,

      ColumnsCount
    };

    /**
     * @brief Initialises the Batches Model.
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit MediaPathsModel( QObject * parent = nullptr );

    //! Destructor
    ~MediaPathsModel() override;

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
     * @name Media Paths
     * @{
     **/

    /**
     * @brief Return all media paths.
     *
     * @return Media Paths.
     **/
    [[nodiscard]] const Arinc665::Utils::MediaPaths& mediaPaths() const;

    /**
     * @brief Return Medium Path for Medium Number.
     *
     * @param[in] mediumNumber
     *   Medium Number
     *
     * @return Medium Path for Medium Number.
     **/
    [[nodiscard]] const std::filesystem::path& mediumPath( Arinc665::MediumNumber mediumNumber ) const;

    /**
     * @brief Assigns Medium Path
     *
     * Determines the medium number and add it to the model.
     *
     * @param[in] path
     *   Medium Path
     *
     * @return if addition was successful.
     **/
    bool mediumPath( std::filesystem::path path );

    /**
     * @brief Removes Medium with the given @p index.
     *
     * If the @p index is invalid or out of range, the operation returns without modifying the model.
     *
     * @param[in] index
     *   Index identifying the medium
     **/
    void remove( const QModelIndex &index );

    /**
     * @brief Clears the content of the model.
     **/
    void clear();

    /** @} **/

    /**
     * Returns if the model is complete, i.e. all media paths are provided.
     *
     * @return If the media path model is complete.
     **/
    [[nodiscard]] bool complete() const;

  private:
    //! Media Paths
    Arinc665::Utils::MediaPaths mediaPathsV;
    //! Media Set Part Number
    std::string partNumberV;
    //! Number of Media Set Members
    Arinc665::MediumNumber numberOfMediaSetMembersV;
};

}

#endif
