// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::SupportedArinc665VersionModel.
 **/

#ifndef ARINC_665_QT_SUPPORTEDARINC665VERSIONMODEL_HPP
#define ARINC_665_QT_SUPPORTEDARINC665VERSIONMODEL_HPP

#include <arinc_665_qt/Arinc665Qt.hpp>

#include <arinc_665/Arinc665.hpp>

#include <QAbstractListModel>

namespace Arinc665Qt {

/**
 * @brief Supported ARINC 665 Version Qt Model.
 **/
class ARINC_665_QT_EXPORT SupportedArinc665VersionModel final : public QAbstractListModel
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Batches Model.
     *
     * @param[in] parent
     *   Parent QObject.
     **/
    explicit SupportedArinc665VersionModel( QObject * parent = nullptr );

    //! Destructor
    ~SupportedArinc665VersionModel() override;

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
     * @brief Return Supported ARINC 665 Version for @p index.
     *
     * @param[in] index
     *   Model Index
     *
     * @return File Creation Policy for @p index
     * @retval {}
     *   If the @p index is out of range.
     **/
    [[nodiscard]] std::optional< Arinc665::SupportedArinc665Version > supportedArinc665Version(
      const QModelIndex &index ) const;

    /**
     * @brief Return Supported ARINC 665 Version for @p index.
     *
     * @param[in] index
     *   Row Index
     *
     * @return File Creation Policy for @p index
     * @retval {}
     *   If the @p index is out of range.
     **/
    [[nodiscard]] std::optional< Arinc665::SupportedArinc665Version > supportedArinc665Version( int index ) const;

    /**
     * @brief Returns Index from ARINC 665 Version value.
     *
     * @param[in] version
     *   ARINC 665 Version Policy
     *
     * @return corresponding index value.
     * @retval -1
     *   If ARINC 665 Version value is invalid.
     **/
    [[nodiscard]] int supportedArinc665Version( Arinc665::SupportedArinc665Version version ) const;
};

}

#endif
