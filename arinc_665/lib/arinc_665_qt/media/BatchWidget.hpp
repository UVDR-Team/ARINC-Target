// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::BatchWidget.
 **/

#ifndef ARINC_665_QT_MEDIA_BATCHWIDGET_HPP
#define ARINC_665_QT_MEDIA_BATCHWIDGET_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <QGroupBox>

#include <memory>

namespace Arinc665Qt::Media {

namespace Ui {
class BatchWidget;
}

/**
 * @brief Widget for Batch Presentation.
 *
 * This widget displays the load-specific information like:
 * - Batch Part Number,
 * - Comment, and
 * - List of Target and Load information.
 **/
class ARINC_665_QT_EXPORT BatchWidget final : public QGroupBox
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Batch Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit BatchWidget( QWidget * parent = nullptr );

    //! Destructor
    ~BatchWidget() override;

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
     * @brief Called when a batch has been selected.
     *
     * The widget is updated with the information of the current Batch.
     *
     * @param[in] batch
     *   Selected Batch
     **/
    void selectBatch( Arinc665::Media::ConstBatchPtr batch );

  private slots:
    /**
     * @brief Slot called when the user selects a Target.
     *
     * Fills the Load List with the loads assigned to the selected target.
     *
     * @param[in] index
     *   Model Index of the selected Target
     **/
    void selectTarget( const QModelIndex &index );

    /**
     * @brief Slot called when the user selects a Load.
     *
     * Gets the selected load and emits the signal @ref activatedLoad().
     *
     * @param[in] index
     *   Model Index of selected Load
     **/
    void activateLoad( const QModelIndex &index );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::BatchWidget > uiV;

    //! Batch Target Information Model
    std::unique_ptr< BatchInfoModel > batchInfoModelV;
    //! Batch Target Loads Model
    std::unique_ptr< LoadsModel > targetLoadsModelV;

    //! Batch
    Arinc665::Media::ConstBatchPtr batchV;
};

}

#endif
