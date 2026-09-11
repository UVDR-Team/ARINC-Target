// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class HelperQt::AboutDialog.
 **/

#ifndef HELPER_QT_ABOUTDIALOG_HPP
#define HELPER_QT_ABOUTDIALOG_HPP

#include <helper_qt/HelperQt.hpp>
#include <helper_qt/VersionsModel.hpp>

#include <QDialog>

#include <memory>

namespace HelperQt {

namespace Ui {
class AboutDialog;
}

//! About Dialog.
class HELPER_QT_EXPORT AboutDialog final : public QDialog
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Dialog
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit AboutDialog( QWidget * parent = nullptr );

    //! Destructor
    ~AboutDialog() override;

    /**
     * @brief Updates the Product Logo.
     *
     * @param[in] pixmap
     *   Product logo pixmap.
     **/
    void productLogo( const QPixmap &pixmap );

    /**
     * @brief Updates the Product Name.
     *
     * @param[in] productName
     *   Product Name
     **/
    void productName( const QString &productName );

    /**
     * @brief Updates the Product Version.
     *
     * @param[in] productVersion
     *   Product Version
     **/
    void productVersion( const QString &productVersion );

    /**
     * @brief Updates the Product License.
     *
     * @param[in] productLicense
     *   Product License
     **/
    void productLicense( const QString &productLicense );

    /**
     * @brief Updates the Product URL.
     *
     * @param[in] productUrl
     *   Product URL
     **/
    void productUrl( const QString &productUrl );

    /**
     * @brief Updates the Versions Information.
     *
     * @param[in] information
     *   Versions information
     **/
    void versions( Helper::VersionsInformation information );

  private slots:
    /**
     * @brief Handler called when user performs double-click on versions entry.
     *
     * If the index is pointing to a URL column entry, the URL is opened in a web browser.
     *
     * @param[in] index
     *   Current model index.
     **/
    void versionDoubleClicked( const QModelIndex &index );

  private:
    //! Qt Designer Class Instance
    std::unique_ptr< Ui::AboutDialog > ui;
    //! Versions Model
    std::unique_ptr< HelperQt::VersionsModel > versionsModelV;
};

}

#endif
