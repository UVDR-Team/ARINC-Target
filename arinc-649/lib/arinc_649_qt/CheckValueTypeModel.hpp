// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc649Qt::CheckValueTypeModel.
 **/

#ifndef ARINC_649_QT_CHECKVALUETYPEMODEL_H
#define ARINC_649_QT_CHECKVALUETYPEMODEL_H

#include <arinc_649_qt/Arinc649Qt.hpp>

#include <arinc_649/Arinc649.hpp>

#include <QAbstractListModel>

#include <optional>

namespace Arinc649Qt {

/**
 * @brief Check Value Type Qt List Model.
 *
 * This model provides the list of check values.
 * The model can be used to fill drop-down fields for selecting a check value.
 * This model only provides a single-column list of check values.
 **/
class ARINC_649_QT_EXPORT CheckValueTypeModel final : public QAbstractListModel
{
    Q_OBJECT

  public:
    /**
     * @name Check Value Conversion
     * @{
     **/

    /**
     * @brief Return Check Value Type for @p index.
     *
     * This method checks the index _row_ to be in range of the check values and returns the actual check value type.
     * The _column_ of the index is ignored.
     *
     * @param[in] index
     *   Model Index
     *
     * @return Check Value Type for @p index
     * @retval std::nullopt
     *   When the @p index is out of range.
     *
     * @sa CheckValueTypeModel::checkValueType(int)
     * @sa CheckValueTypeModel::checkValueType(Arinc649::CheckValueType)
     **/
    [[nodiscard]] static std::optional< Arinc649::CheckValueType > checkValueType( const QModelIndex &index );

    /**
     * @brief Return Check Value Type for @p index.
     *
     * @param[in] index
     *   Row Index
     *
     * @return Check Value Type for @p index
     * @retval std::nullopt
     *   When the @p index is out of range.
     *
     * @sa CheckValueTypeModel::checkValueType(const QModelIndex&)
     * @sa CheckValueTypeModel::checkValueType(Arinc649::CheckValueType)
     **/
    [[nodiscard]] static std::optional< Arinc649::CheckValueType > checkValueType( int index );

    /**
     * @brief Returns Index from Check Value Type value.
     *
     * @param[in] type
     *   Check Value Type
     *
     * @return corresponding index value.
     * @retval -1
     *   If Check Value Type value is invalid.
     *
     * @sa CheckValueTypeModel::checkValueType(int)
     * @sa CheckValueTypeModel::checkValueType(const QModelIndex&)
     **/
    [[nodiscard]] static int checkValueType( Arinc649::CheckValueType type );

    /** @} **/

    /**
     * @brief Initialises the Batches Model.
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit CheckValueTypeModel( QObject * parent = nullptr );

    //! Destructor
    ~CheckValueTypeModel() override = default;

    /**
     * @brief Returns the number of rows.
     *
     * The model returns the fixed size of enumeration elements.
     *
     * @param[in] parent
     *   Index-parent - assumed to be the root element (invalid).
     *
     * @return Number of Check Value Types.
     * @retval 0
     *   If @p parent is valid (not root element)
     **/
    [[nodiscard]] int rowCount( const QModelIndex &parent ) const override;

    /**
     * @brief Returns the requested data.
     *
     * @param[in] index
     *   Index of the requested item.
     * @param[in] role
     *   The role for which data is retrieved.
     *   Only _Qt::DisplayRole_ is supported.
     *
     * @return Data dependent of the index and role.
     * @retval QVariant{}
     *   When parameters are out of range.
     **/
    [[nodiscard]] QVariant data( const QModelIndex &index, int role ) const override;

    /**
     * @brief Provides Header Data for the Model.
     *
     * This function returns the data to be displayed in the headers of the model.
     *
     * @param[in] section
     *   The section index of the header.
     * @param[in] orientation
     *   The orientation of the header, either horizontal or vertical.
     * @param[in] role
     *   The role for which data is retrieved.
     *   Only _Qt::DisplayRole_ is supported.
     *
     * @return The data to be displayed in the specified header section.
     * @retval QVariant{}
     *   When parameters are out of range.
     **/
    [[nodiscard]] QVariant headerData(
      int section,
      Qt::Orientation orientation,
      int role = Qt::ItemDataRole::DisplayRole ) const override;
};

}

#endif
