// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::MediaSetViewWidget.
 **/

#ifndef ARINC_665_QT_MEDIA_MEDIASETVIEWWIDGET_HPP
#define ARINC_665_QT_MEDIA_MEDIASETVIEWWIDGET_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QWidget>

#include <memory>

namespace Arinc665Qt::Media {

namespace Ui {
class MediaSetViewWidget;
}

/**
 * @brief Widget for %Media Set Presentation.
 *
 * This provides a split view of a tree and a detailed window.
 **/
class ARINC_665_QT_EXPORT MediaSetViewWidget final : public QWidget
{
    Q_OBJECT

  public:
    //! Enumeration, which identifies the selected type of Widget.
    enum class DetailsStackedWidget : int
    {
      MediaSet = 0,
      Directory = 1,
      File = 2
    };

    /**
     * @brief Initialises the Media Set Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit MediaSetViewWidget( QWidget * parent = nullptr );

    //! Destructor
    ~MediaSetViewWidget() override;

    /**
     * @brief Sets the Media Set Model.
     *
     * @param[in] model
     *   Media Set Model.
     **/
    void mediaSetModel( MediaSetModel * model );

  private slots:
    /**
     * @brief Called when a selection has been made within tree-view.
     *
     * @param[in] index
     *   Index of the selected element.
     **/
    void selectElement( const QModelIndex &index );

    /**
     * @brief Slot called, when an Element (Directory, Regular File, Load, Batch) has been selected.
     *
     * The given element is focused within the tree view.
     *
     * @param[in] element
     *   Element to activate
     **/
    void activateElement( Arinc665::Media::ConstBasePtr element );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::MediaSetViewWidget > uiV;
    //! Media Set Model
    MediaSetModel * mediaSetModelV{ nullptr };
};

}

#endif
