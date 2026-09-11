// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::RegularFileWidget.
 **/

#ifndef ARINC_665_QT_MEDIA_REGULARFILEWIDGET_HPP
#define ARINC_665_QT_MEDIA_REGULARFILEWIDGET_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QGroupBox>

#include <memory>

namespace Arinc665Qt::Media {

namespace Ui {
class RegularFileWidget;
}

/**
 * @brief Widget for Regular File Presentation.
 **/
class ARINC_665_QT_EXPORT RegularFileWidget final : public QGroupBox
{
  Q_OBJECT

  public:
    /**
     * @brief Initialises the Regular File Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit RegularFileWidget( QWidget * parent = nullptr );

    //! Destructor
    ~RegularFileWidget() override;

  signals:
    /**
     * @brief Signal Emitted when the user selected a Load.
     *
     * @param[in] load
     *   Selected Load.
     **/
    void activatedLoad( Arinc665::Media::ConstLoadPtr load );

  public slots:
    /**
     * @brief Called when a File has been selected.
     *
     * @param[in] file
     *   Selected File
     **/
    void selectFile( Arinc665::Media::ConstRegularFilePtr file );

  private slots:
    /**
     * @brief Slot called when the user selects a Load.
     *
     * Gets the load from the given index and emit the signal @ref activatedLoad().
     *
     * @param[in] index
     *   Model Index of selected Load
     **/
    void activateLoad( const QModelIndex &index );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::RegularFileWidget > uiV;
    //! Used in Loads Model
    std::unique_ptr< LoadsModel > usedInLoadsModelV;

    //! File
    Arinc665::Media::ConstRegularFilePtr fileV;
};

}

#endif
