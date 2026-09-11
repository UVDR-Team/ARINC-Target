// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class
 *   Arinc665Qt::MediaSetManager::MediaSetManagerWindow.
 **/

#ifndef ARINC_665_QT_MEDIA_SET_MANAGER_MEDIASETMANAGERDIALOG_HPP
#define ARINC_665_QT_MEDIA_SET_MANAGER_MEDIASETMANAGERDIALOG_HPP

#include <arinc_665_qt/media_set_manager/MediaSetManager.hpp>

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <helper_qt/HelperQt.hpp>

#include <QMainWindow>

#include <memory>

namespace Arinc665Qt::MediaSetManager {

namespace Ui {
class MediaSetManagerWindow;
}

/**
 * @brief %Media Set Manager Window
 *
 * The Media Set Manager Window provides listing of the Media Sets within the Media Set manager and provides operations
 * onto the Media Sets:
 * - View Media Set,
 * - Importing Media Sets,
 * - Import Media Set XML Configuration, and
 * - Removing Media Sets.
 **/
class ARINC_665_QT_EXPORT MediaSetManagerWindow final : public QMainWindow
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Media Set Manager Window.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit MediaSetManagerWindow( QWidget * parent = nullptr );

    //! Destructor
    ~MediaSetManagerWindow() override;

  public slots:
    /**
     * @brief Assigns Media Set Manager.
     *
     * Reloads the Media Sets from the Media Set manager and updates the Media Sets Model.
     **/
    void mediaSetManger( Arinc665::Utils::MediaSetManagerPtr mediaSetManager );

    /**
     * @brief Reloads the Media Sets from the Media Set manager and updates the Media Sets Model.
     **/
    void reloadMediaSetModel();

  private slots:
    /**
     * @brief Slot handling View Media Set Clicked.
     **/
    void viewMediaSet();

    /**
     * @brief Import Media Set Signal
     **/
    void importMediaSet();

    /**
     * @brief Import Media Set XML Signal
     **/
    void importMediaSetXml();

    /**
     * @brief Slot handling Remove Media Set Clicked.
     **/
    void removeMediaSet();

    /**
     * @brief Slot handling the Open Media Sets Directory
     */
    void openMediaSetsDirectory();

    /**
     * @brief Open Settings Dialog
     **/
    void showSettings();

    /**
     * @brief Save Media Set Manager Settings.
     **/
    void saveSettings();

  private:
    //! UI (designer)
    std::unique_ptr< Ui::MediaSetManagerWindow > uiV;

    //! View Media Set Dialog
    std::unique_ptr< ViewMediaSetDialog > viewMediaSetDialogV;
    //! Settings Dialog
    std::unique_ptr< MediaSetManagerSettingsDialog > settingsDialogV;
    //! About Dialog
    std::unique_ptr< HelperQt::AboutDialog > aboutDialogV;

    //! Media Set Manager
    Arinc665::Utils::MediaSetManagerPtr mediaSetManagerV;
    //! Media Set Model
    std::unique_ptr< Media::MediaSetsModel > mediaSetsModelV;
};

}

#endif
