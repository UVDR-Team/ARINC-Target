// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::MediaSetWidget.
 **/

#include "MediaSetWidget.hpp"

#include "ui_MediaSetWidget.h"

#include <arinc_665_qt/media/LoadsModel.hpp>
#include <arinc_665_qt/media/BatchesModel.hpp>
#include <arinc_665_qt/media/MediaSetModel.hpp>

#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/Batch.hpp>

#include <arinc_649_qt/CheckValueTypeModel.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

MediaSetWidget::MediaSetWidget( QWidget * const parent ):
  QWidget{ parent },
  uiV{ std::make_unique< Ui::MediaSetWidget>() },
  checkValueTypeModelV{ std::make_unique< Arinc649Qt::CheckValueTypeModel >( this ) },
  loadsModelV{ std::make_unique< Media::LoadsModel >( this ) },
  batchesModelV{ std::make_unique< Media::BatchesModel >( this ) }
{
  uiV->setupUi( this );

  uiV->mediaSetCheckValueType->setModel( checkValueTypeModelV.get() );
  uiV->listOfFilesCheckValueType->setModel( checkValueTypeModelV.get() );
  uiV->listOfLoadsCheckValueType->setModel( checkValueTypeModelV.get() );
  uiV->listOfBatchesCheckValueType->setModel( checkValueTypeModelV.get() );
  uiV->filesCheckValueType->setModel( checkValueTypeModelV.get() );

  uiV->loads->setModel( loadsModelV.get() );
  uiV->batches->setModel( batchesModelV.get() );

  uiV->content->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );
  uiV->loads->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );
  uiV->batches->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  connect(
    uiV->content,
    &QTableView::activated,
    this,
    &MediaSetWidget::selectElement );
  connect(
    uiV->loads,
    &QTableView::activated,
    this,
    &MediaSetWidget::selectLoad );
  connect(
    uiV->batches,
    &QTableView::activated,
    this,
    &MediaSetWidget::selectBatch );
}

MediaSetWidget::~MediaSetWidget() = default;

void MediaSetWidget::mediaSetModel( MediaSetModel * const model )
{
  mediaSetModelV = model;
  uiV->content->setModel( model );
}

void MediaSetWidget::selectMediaSet( Arinc665::Media::ConstMediaSetPtr mediaSet )
{
  mediaSetV = std::move( mediaSet );

  if ( mediaSetV )
  {
    uiV->partNumberLineEdit->setText( HelperQt::toQString( mediaSetV->partNumber() ) );

    uiV->defaultMediumNumber->setValue( static_cast< uint8_t >( mediaSetV->effectiveDefaultMediumNumber() ) );

    uiV->mediaSetCheckValueTypeGroupBox->setChecked( mediaSetV->mediaSetCheckValueType().has_value() );
    uiV->mediaSetCheckValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( mediaSetV->effectiveMediaSetCheckValueType() ) );

    uiV->listOfFilesCheckValueTypeGroupBox->setChecked( mediaSetV->listOfFilesCheckValueType().has_value() );
    uiV->listOfFilesCheckValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( mediaSetV->effectiveListOfFilesCheckValueType() ) );

    uiV->listOfLoadsCheckValueTypeGroupBox->setChecked( mediaSetV->listOfLoadsCheckValueType().has_value() );
    uiV->listOfLoadsCheckValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( mediaSetV->effectiveListOfLoadsCheckValueType() ) );

    uiV->listOfBatchesCheckValueTypeGroupBox->setChecked( mediaSetV->listOfBatchesCheckValueType().has_value() );
    uiV->listOfBatchesCheckValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( mediaSetV->effectiveListOfBatchesCheckValueType() ) );

    uiV->filesCheckValueTypeGroupBox->setChecked( mediaSetV->filesCheckValueType().has_value() );
    uiV->filesCheckValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( mediaSetV->effectiveFilesCheckValueType() ) );

    uiV->content->setRootIndex( mediaSetModelV->index( 0, 0 ) );

    loadsModelV->loads( mediaSetV->recursiveLoads() );

    batchesModelV->batches( mediaSetV->recursiveBatches() );
  }
}

void MediaSetWidget::selectElement( const QModelIndex &index )
{
  if ( auto element{ mediaSetModelV->element( index ) }; element )
  {
    emit activatedElement( std::move( element ) );
  }
}

void MediaSetWidget::selectLoad( const QModelIndex &index )
{
  if ( auto load{ loadsModelV->constLoad( loadsModelV->load( index ) ) }; load )
  {
    emit activatedElement( std::move( load ) );
  }
}

void MediaSetWidget::selectBatch( const QModelIndex &index )
{
  if ( auto batch{ batchesModelV->constBatch( batchesModelV->batch( index ) ) }; batch )
  {
    emit activatedElement( std::move( batch ) );
  }
}

}
