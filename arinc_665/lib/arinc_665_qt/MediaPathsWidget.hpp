// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::MediaPathsWidget.
 **/

#ifndef ARINC_665_QT_MEDIAPATHSPAGE_HPP
#define ARINC_665_QT_MEDIAPATHSPAGE_HPP

#include <arinc_665_qt/Arinc665Qt.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QGroupBox>
#include <QFileDialog>

#include <memory>

namespace Arinc665Qt {

namespace Ui{
class MediaPathsWidget;
}

/**
 * @brief Media Paths Widget.
 *
 * This Widget provides a list of media paths.
 * Actions are provided to add and remove media.
 * The media are held by a @ref MediaPathsModel.
 **/
class ARINC_665_QT_EXPORT MediaPathsWidget final : public QGroupBox
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit MediaPathsWidget( QWidget * parent = nullptr );

    //! Destructor
    ~MediaPathsWidget() override;

    /**
     * @brief Returns, if the Media Paths are complete
     *
     * @return If the Media Paths are complete.
     **/
    [[nodiscard]] bool completed() const;

    /**
     * @brief Clears all entries in the media path model
     **/
    void clear();

  signals:
    /**
     * @brief Signal emitted when the user adds or delete media paths.
     *
     * @param[in] mediaPaths
     *   Updated Media Paths
     **/
    void mediaPathsChanged( const Arinc665::Utils::MediaPaths &mediaPaths );

  private slots:
    /**
     * @brief Slot called when the selection has changed.
     *
     * @param[in] current
     *   Current selected item.
     **/
    void updateButtons( const QModelIndex &current );

    /**
     * @brief Slot when the user wants to delete a selected medium directory.
     *
     * This slot is connected to the delete-button QPushButton::pressed signal.
     * The selected medium is removed from the model.
     **/
    void removeMediumDirectory();

    /**
     * @brief Slot called when an input directory has been selected.
     *
     * This slot is connected to the QFileDialog::fileSelected signal.
     * This slot checks if the selected directory is a media directory and adds it to the model.
     *
     * @param[in] file
     *   Selected File
     **/
    void mediumDirectorySelected( const QString &file );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::MediaPathsWidget > uiV;
    //! Media Paths Model
    std::unique_ptr< MediaPathsModel > mediaPathsModelV;
    //! Select XML File Dialog
    std::unique_ptr< QFileDialog > selectMediaPathDialogV;
};

}

#endif
