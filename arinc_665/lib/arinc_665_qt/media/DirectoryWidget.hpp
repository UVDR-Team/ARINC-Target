// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::Directory Widget.
 **/

#ifndef ARINC_665_QT_MEDIA_DIRECTORYWIDGET_HPP
#define ARINC_665_QT_MEDIA_DIRECTORYWIDGET_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QWidget>

#include <memory>

namespace Arinc665Qt::Media {

namespace Ui {
class DirectoryWidget;
}

//! Widget for Directory Presentation
class ARINC_665_QT_EXPORT DirectoryWidget final : public QWidget
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Directory Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit DirectoryWidget( QWidget * parent = nullptr );

    //! Destructor
    ~DirectoryWidget() override;

    /**
     * @brief Sets the Media Set Model.
     *
     * @param[in] model
     *   Media Set Model.
     **/
    void mediaSetModel( MediaSetModel * model );

  signals:
    /**
     * @brief Emitted when an element in one of the widgets is activated.
     *
     * @param element
     **/
    void activatedElement( Arinc665::Media::ConstBasePtr element );

  public slots:
    /**
     * @brief Slot called when a Directory is selected.
     *
     * Updated the Table View to show the content of the selected directory.
     *
     * @param[in] index
     *   Model Index of the selected directory.
     **/
    void selectDirectory( const QModelIndex &index );

    /**
     * @brief Called when a Directory has been selected.
     *
     * @param[in] directory
     *   Selected Directory
     **/
    void selectDirectory( Arinc665::Media::ConstDirectoryPtr directory );

  private slots:
    /**
     * @brief Slot Called when an Element has been selected
     *
     * @param[in] index
     *   Selected model element.
     **/
    void selectElement( const QModelIndex &index );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::DirectoryWidget > uiV;
    //! Media Set Model
    MediaSetModel * mediaSetModelV{ nullptr };
    //! Directory
    Arinc665::Media::ConstDirectoryPtr directoryV;
};

}

#endif
