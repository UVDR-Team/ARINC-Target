// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::MediaSetManager::ViewMediaSetDialog.
 **/

#ifndef ARINC_665_QT_MEDIA_SET_MANAGER_VIEWMEDIASETDIALOG_HPP
#define ARINC_665_QT_MEDIA_SET_MANAGER_VIEWMEDIASETDIALOG_HPP

#include <arinc_665_qt/media_set_manager/MediaSetManager.hpp>

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QDialog>

#include <memory>

namespace Arinc665Qt::MediaSetManager {

namespace Ui{
class ViewMediaSetDialog;
}

//! View %Media Set Dialog
class ARINC_665_QT_EXPORT ViewMediaSetDialog final : public QDialog
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Media Set Dialog.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit ViewMediaSetDialog( QWidget * parent = nullptr );

    //! Destructor
    ~ViewMediaSetDialog() override;

    /**
     * @brief Sets the Media Set Model.
     *
     * @param[in] mediaSet
     *   Media Set.
     **/
    void mediaSet( Arinc665::Media::ConstMediaSetPtr mediaSet );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::ViewMediaSetDialog > uiV;
    //! Media Set Model
    std::unique_ptr< Media::MediaSetModel > mediaSetModelV;
};

}

#endif
