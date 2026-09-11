// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class HelperQt::EndianModel.
 **/

#ifndef HELPER_QT_ENDIANMODEL_HPP
#define HELPER_QT_ENDIANMODEL_HPP

#include <helper_qt/HelperQt.hpp>

#include <QAbstractListModel>

#include <optional>

namespace HelperQt {

/**
 * @brief A model derived from QAbstractListModel to represent endian order information.
 *
 * The EndianModel provides data and header information required to display and interact with a list of endian
 * descriptions.
 **/
class HELPER_QT_EXPORT EndianModel final : public QAbstractListModel
{
    Q_OBJECT

  public:
    /**
     * @brief Returns the Endian Information for the given Model Index.
     *
     * @param[in] index
     *   Model Index row.
     *
     * @return Endian information for the given Model Index.
     * @retval {}
     *   When Model Index is invalid or out of range.
     **/
    [[nodiscard]] static std::optional< std::endian > endian( int index );

    /**
     * @brief Returns the index to the corresponding endian value.
     *
     * @param[in] endian
     *   Endian value.
     *
     * @return Corresponding index.
     * @retval {}
     *   When parameter is invalid or out of range.
     **/
    [[nodiscard]] static std::optional< int > index( std::endian endian );

    /**
     * @brief Constructs the Model.
     *
     * @param[in] parent
     *   Parent QObject
     **/
    explicit EndianModel( QObject * parent = nullptr );

    //! Destructor
    ~EndianModel() override;

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
     * @return
     *   Header data for the given parameters.
     **/
    [[nodiscard]] QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
};

}

#endif
