// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class HelperQt::VersionsModel.
 **/

#ifndef HELPER_QT_VERSIONSMODEL_HPP
#define HELPER_QT_VERSIONSMODEL_HPP

#include <helper_qt/HelperQt.hpp>

#include <helper/VersionInformation.hpp>

#include <QAbstractTableModel>

namespace HelperQt {

/**
 * @brief Versions Qt Table Model.
 *
 * This model is used to provide a list of products with their version
 * information.
 **/
class HELPER_QT_EXPORT VersionsModel final : public QAbstractTableModel
{
    Q_OBJECT

  public:
    //! Columns of Model
    enum class Columns : int
    {
      //! Product Name
      Name,
      //! Product Version
      Version,
      //! Product License
      License,
      //! Product URL
      Url,

      //! Columns Count
      ColumnsCount
    };

    /**
     * @brief Constructs the Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit VersionsModel( QObject *parent = nullptr );

    //! Destructor
    ~VersionsModel() override;

    /**
     * @brief Returns the number of rows.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Number of version information.
     * @retval 0
     *   If @p is valid (not root element)
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent = {} ) const override;

    /**
     * @brief Returns the number of columns.
     *
     * @param[in] parent
     *   Parent Model Index.
     *
     * @return Columns::ColumnsCount.
     * @retval 0
     *   If @p is valid (not root element)
     **/
    [[nodiscard]] int columnCount( const QModelIndex &parent = {} ) const override;

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
     *   Section number
     * @param[in] orientation
     *   Orientation
     * @param[in] role
     *   Item role.
     *
     * @return
     *   Header data for the given parameters.
     **/
    [[nodiscard]] QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;

    /**
     * @name Version Information
     * @{
     **/

    /**
     * @brief Returns the Versions Information.
     *
     * @return Version Information.
     **/
    [[nodiscard]] const Helper::VersionsInformation& versions() const;

    /**
     * @brief Updates the Versions information,
     *
     * @param[in] versions
     *   Versions Information
     **/
    void versions( Helper::VersionsInformation versions );

    /**
     * @brief Add Version Information
     *
     * @param[in] key
     *   Product Key
     * @param information
     *   Product Version.
     **/
    void version( std::string_view key, Helper::VersionInformation information );

    /**
     * @brief Returns the Version Information for the given Model Index.
     *
     * @param[in] index
     *   Model Index.
     *
     * @return Version Information for the given Model Index.
     * @retval {}
     *   When Model Index is invalid or out of range.
     **/
    [[nodiscard]] Helper::VersionsInformation::value_type version( const QModelIndex &index ) const;

    /** @} **/

  private:
    //! Versions Information
    Helper::VersionsInformation versionsInformationV;
};

}

#endif
