// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Media::LoadWidget.
 **/

#ifndef ARINC_665_QT_MEDIA_LOADWIDGET_HPP
#define ARINC_665_QT_MEDIA_LOADWIDGET_HPP

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_649_qt/Arinc649Qt.hpp>

#include <QGroupBox>

#include <memory>

namespace Arinc665Qt::Media {

namespace Ui {
class LoadWidget;
}

/**
 * @brief Widget for Load Presentation.
 *
 * This widget displays the load-specific information like:
 * - Load Part Number,
 * - Part Flags,
 * - Load Type and Description,
 * - List of Target Hardware and Positions,
 * - Load Check Value Type,
 * - List of Data and Support files, and
 * - And References to Batches, which contains the Load.
 **/
class ARINC_665_QT_EXPORT LoadWidget final : public QGroupBox
{
  Q_OBJECT

  public:
    /**
     * @brief Initialises the Load Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit LoadWidget( QWidget * parent = nullptr );

    //! Destructor
    ~LoadWidget() override;

  signals:
    /**
     * @brief Signal Emitted when the user selected a Data or Support File.
     *
     * @param[in] file
     *   Selected Data or Support File.
     **/
    void activatedFile( Arinc665::Media::ConstRegularFilePtr file );

    /**
     * @brief Signal Emitted when the user selected a Batch.
     *
     * @param[in] batch
     *   Selected Batch.
     **/
    void activatedBatch( Arinc665::Media::ConstBatchPtr batch );

  public slots:
    /**
     * @brief Called when a Load has been selected.
     *
     * Stores the load internally and updates the elements of the widget to match the new Load.
     *
     * @param[in] load
     *   Selected Load
     **/
    void selectLoad( Arinc665::Media::ConstLoadPtr load );

  private slots:
    /**
     * @brief Slot Called when a Data File has been selected.
     *
     * Gets the selected file and emits the signal @ref activatedFile().
     *
     * @param[in] index
     *   Index of the selected data file.
     **/
    void selectDataFile( const QModelIndex &index );

    /**
     * @brief Slot Called when a Support File has been selected.
     *
     * Gets the selected file and emits the signal @ref activatedFile().
     *
     * @param[in] index
     *   Index of the selected support file.
     **/
    void selectSupportFile( const QModelIndex &index );

    /**
     * @brief Slot Called when a Batch has been selected.
     *
     * Gets the selected batch and emits the signal @ref activatedBatch().
     *
     * @param[in] index
     *   Index of the selected batch.
     **/
    void selectBatch( const QModelIndex &index );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::LoadWidget> uiV;
    //! Check Value Type Model
    std::unique_ptr< Arinc649Qt::CheckValueTypeModel > checkValueTypeModelV;
    //! THW ID Positions Model
    std::unique_ptr< TargetHardwareIdsPositionsModel > targetHardwareIdsPositionsModelV;
    //! Data Files Model
    std::unique_ptr< LoadFilesModel > dataFilesModelV;
    //! Support Files Model
    std::unique_ptr< LoadFilesModel > supportFilesModelV;
    //! Used in Batches Model
    std::unique_ptr< BatchesModel > usedInBatchesModelV;
    //! Load
    Arinc665::Media::ConstLoadPtr loadV;
};

}

#endif
