// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::MediaSetViewerWindow.
 **/

#ifndef ARINC_665_QT_MEDIA_SET_VIEW_WMEDIASETVIEWERWINDOW_HPP
#define ARINC_665_QT_MEDIA_SET_VIEW_WMEDIASETVIEWERWINDOW_HPP

#include <arinc_665_qt/media_set_viewer/MediaSetViewerWindow.hpp>

#include <arinc_665_qt/decompile_media_set/DecompileMediaSet.hpp>

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QMainWindow>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QFileSystemWatcher>

#include <memory>

namespace Arinc665Qt {

namespace Ui {
class MediaSetViewerWindow;
}

/**
 * @brief ARINC 665 %Media Set Viewer Window
 *
 * Shows a window, where an ARINC 665 Media Set structure is shown.
 * The window provides actions to load an ARINC 665 Media Set XML or load an existing ARINC 665 Media set via
 * @ref Arinc665::Utils::FilesystemMediaSetDecompiler.
 **/
class ARINC_665_QT_EXPORT MediaSetViewerWindow final : public QMainWindow
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Media Set Window.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit MediaSetViewerWindow( QWidget * parent = nullptr );

    //! Destructor
    ~MediaSetViewerWindow() override;

  private slots:
    /**
     * @brief Slot called when the user updates the Media Paths.
     *
     * @param[in] mediaPaths
     *   Media Paths
     **/
    void updateMediaPaths( const Arinc665::Utils::MediaPaths &mediaPaths );

    /**
     * @brief Slot called when the user changed the check file integrity handling
     *
     * @param[in] checkFileIntegrity
     *   Check File Integrity indicator
     **/
    void checkFileIntegrity( bool checkFileIntegrity );

    /**
     * @brief Slot for Media Set Decompilation.
     **/
    void startMediaSetDecompilation();


    /**
     * @brief Called, when the user has selected a file.
     *
     * @param[in] file
     *   Selected file.
     **/
    void loadXmlFile( const QString &file );

    /**
     * @brief Save XML file.
     *
     * @param[in] file
     *   XML Filename
     **/
    void saveXmlFile( const QString &file );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::MediaSetViewerWindow > ui;
    //! Wizard Dialog
    std::unique_ptr< DecompileMediaSetWizard > decompileMediaSetWizardV;
    //! Select Load Media Set XML file dialog
    std::unique_ptr< QFileDialog > selectLoadMediaSetXmlDialogV;
    //! Select Save Media Set XML file dialog
    std::unique_ptr< QFileDialog > selectSaveMediaSetXmlDialogV;

    //! Media Set Model
    std::unique_ptr< Media::MediaSetModel > mediaSetModelV;
    //! File Path Mapping Model
    std::unique_ptr< FilePathMappingModel > filePathMappingModelV;
    //! Sorted File Mapping Model
    std::unique_ptr< QSortFilterProxyModel > sortedFilePathMappingModelV;
    //! Filesystem Watcher
    std::unique_ptr< QFileSystemWatcher > fileSystemWatcherV;

    //! Media Paths
    Arinc665::Utils::MediaPaths mediaPathsV;
    //! Check File Integrity
    bool checkFileIntegrityV{ false };
};

}

#endif
