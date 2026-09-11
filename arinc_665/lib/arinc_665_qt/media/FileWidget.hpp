// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::FileWidget.
 **/

#ifndef ARINC_665_QT_MEDIA_FILEWIDGET_HPP
#define ARINC_665_QT_MEDIA_FILEWIDGET_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_649_qt/Arinc649Qt.hpp>

#include <QWidget>

#include <memory>

namespace Arinc665Qt::Media {

namespace Ui {
class FileWidget;
}

/**
 * @brief Widget for handling ARINC 665 Media Set files.
 *
 * Displays the filename, medium number, and check value type.
 * Distinct between:
 * - Regular file,
 * - Load, and
 * - Batch.
 * For each filetype a specific widget is displayed.
 *
 * @sa @ref RegularFileWidget
 * @sa @ref LoadWidget
 * @sa @ref BatchWidget
 **/
class ARINC_665_QT_EXPORT FileWidget final : public QWidget
{
    Q_OBJECT

  public:
    //! Enumeration which identifies the Widgets
    enum class FileStackedWidget : int
    {
      RegularFile = 0,
      LoadFile = 1,
      BatchFile = 2
    };

    /**
     * @brief Initialises the File Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit FileWidget( QWidget * parent = nullptr );

    //! Destructor
    ~FileWidget() override;

  signals:
    /**
     * @brief Emitted when a file in one of the widgets is activated.
     *
     * @param[in] file
     *   Activated file.
     **/
    void activatedFile( Arinc665::Media::ConstFilePtr file );

  public slots:
    /**
     * @brief Called when a file has been selected.
     *
     * When in the child widgets a file (regular file, load, or batch) has been activated, the selected file is emitted.
     *
     * @param[in] file
     *   Selected File
     **/
    void selectFile( Arinc665::Media::ConstFilePtr file );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::FileWidget > uiV;
    //! Check Value Type Model
    std::unique_ptr< Arinc649Qt::CheckValueTypeModel > checkValueTypeModelV;
    //! File
    Arinc665::Media::ConstFilePtr fileV;
};

}

#endif
