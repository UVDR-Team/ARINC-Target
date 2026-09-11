// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::LoadWidget.
 **/

#include "LoadWidget.hpp"

#include "ui_LoadWidget.h"

#include <arinc_665_qt/media/BatchesModel.hpp>
#include <arinc_665_qt/media/LoadFilesModel.hpp>
#include <arinc_665_qt/media/TargetHardwareIdsPositionsModel.hpp>

#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/RegularFile.hpp>
#include <arinc_665/media/MediaSet.hpp>

#include <arinc_649_qt/CheckValueTypeModel.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

LoadWidget::LoadWidget( QWidget * const parent):
  QGroupBox{ parent },
  uiV{ std::make_unique< Ui::LoadWidget>() },
  checkValueTypeModelV{ std::make_unique< Arinc649Qt::CheckValueTypeModel >( this ) },
  targetHardwareIdsPositionsModelV{ std::make_unique< TargetHardwareIdsPositionsModel >( this ) },
  dataFilesModelV{ std::make_unique< LoadFilesModel >( this ) },
  supportFilesModelV{ std::make_unique< LoadFilesModel >( this ) },
  usedInBatchesModelV{ std::make_unique< BatchesModel >( this ) }
{
  uiV->setupUi( this );

  uiV->targetHardwareIdsPositions->setModel( targetHardwareIdsPositionsModelV.get() );
  uiV->dataFiles->setModel( dataFilesModelV.get() );
  uiV->supportFiles->setModel( supportFilesModelV.get() );
  uiV->usedInBatches->setModel( usedInBatchesModelV.get() );

  uiV->loadCheckValueType->setModel( checkValueTypeModelV.get() );

  uiV->dataFilesCheckValueType->setModel( checkValueTypeModelV.get() );
  uiV->dataFiles->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  uiV->supportFilesCheckValueType->setModel( checkValueTypeModelV.get() );
  uiV->supportFiles->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  uiV->usedInBatches->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  connect(
    uiV->dataFiles,
    &QTableView::activated,
    this,
    &LoadWidget::selectDataFile );
  connect(
    uiV->supportFiles,
    &QTableView::activated,
    this,
    &LoadWidget::selectSupportFile );
  connect(
    uiV->usedInBatches,
    &QTableView::activated,
    this,
    &LoadWidget::selectBatch );
}

LoadWidget::~LoadWidget() = default;

void LoadWidget::selectLoad( Arinc665::Media::ConstLoadPtr load )
{
  loadV = std::move( load );

  if ( loadV )
  {
    uiV->partNumber->setText( HelperQt::toQString( loadV->partNumber() ) );

    uiV->partFlags->setText( QString::number( loadV->partFlags(), 16 ) );

    uiV->gbLoadType->setChecked( loadV->loadType().has_value() );
    uiV->loadTypeDescription->setText( "" );
    uiV->loadTypeId->setText( "" );
    if ( const auto loadType{ loadV->loadType() }; loadType )
    {
      uiV->loadTypeDescription->setText( QString::fromStdString( loadType->first ) );
      uiV->loadTypeId->setText( QString::number( loadType->second ) );
    }

    targetHardwareIdsPositionsModelV->targetHardwareIdsPositions( loadV->targetHardwareIdPositions() );

    uiV->loadCheckValueTypeGroupBox->setChecked( loadV->loadCheckValueType().has_value() );
    uiV->loadCheckValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( loadV->effectiveLoadCheckValueType() ) );

    uiV->dataFilesCheckValueTypeGroupBox->setChecked( loadV->dataFilesCheckValueType().has_value() );
    uiV->dataFilesCheckValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( loadV->effectiveDataFilesCheckValueType() ) );
    dataFilesModelV->loadFiles( loadV->dataFiles() );

    uiV->supportFilesCheckValueTypeGroupBox->setChecked( loadV->supportFilesCheckValueType().has_value() );
    uiV->supportFilesCheckValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( loadV->effectiveSupportFilesCheckValueType() ) );
    supportFilesModelV->loadFiles( loadV->supportFiles() );

    usedInBatchesModelV->batches( loadV->mediaSet()->batchesWithLoad( loadV ) );
  }
}

void LoadWidget::selectDataFile( const QModelIndex &index )
{
  if ( const auto file{ dataFilesModelV->loadFile( index ) }; file )
  {
    emit activatedFile( std::get< 0 >( *file ) );
  }
}

void LoadWidget::selectSupportFile( const QModelIndex &index )
{
  if ( const auto file{ supportFilesModelV->loadFile( index ) }; file )
  {
    emit activatedFile( std::get< 0 >( *file ) );
  }
}

void LoadWidget::selectBatch( const QModelIndex &index )
{
  if ( const auto batch{ usedInBatchesModelV->constBatch( usedInBatchesModelV->batch( index ) ) }; batch )
  {
    emit activatedBatch( batch );
  }
}

}
