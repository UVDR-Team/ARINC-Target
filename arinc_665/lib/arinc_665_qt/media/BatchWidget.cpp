// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::BatchWidget.
 **/

#include "BatchWidget.hpp"

#include "ui_BatchWidget.h"

#include <arinc_665_qt/media/BatchInfoModel.hpp>
#include <arinc_665_qt/media/LoadsModel.hpp>

#include <arinc_665/media/Batch.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

BatchWidget::BatchWidget( QWidget * const parent ) :
  QGroupBox{ parent},
  uiV{ std::make_unique< Ui::BatchWidget>() },
  batchInfoModelV{ std::make_unique< BatchInfoModel >( this ) },
  targetLoadsModelV{ std::make_unique< LoadsModel >( this ) }
{
  uiV->setupUi( this );

  uiV->targets->setModel( batchInfoModelV.get() );
  uiV->loads->setModel( targetLoadsModelV.get() );
  uiV->loads->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  connect(
    uiV->targets->selectionModel(),
    &QItemSelectionModel::currentChanged,
    this,
    &BatchWidget::selectTarget );
  connect(
    uiV->loads,
    &QTableView::activated,
    this,
    &BatchWidget::activateLoad );
}

BatchWidget::~BatchWidget() = default;


void BatchWidget::selectBatch( Arinc665::Media::ConstBatchPtr batch )
{
  batchV = std::move( batch );

  if ( batchV )
  {
    uiV->partNumber->setText( HelperQt::toQString( batchV->partNumber() ) );
    uiV->comment->setText( HelperQt::toQString( batchV->comment() ) );

    batchInfoModelV->batchInformation( batchV->targets() );
    targetLoadsModelV->loads( {} );

    uiV->targets->selectRow( 0 );
  }
}

void BatchWidget::selectTarget( const QModelIndex &index )
{
  const auto batchTargetInformation{ batchInfoModelV->batchTargetInformation( index ) };

  targetLoadsModelV->loads( batchInfoModelV->constBatchTargetInformation( batchTargetInformation ).second );
}

void BatchWidget::activateLoad( const QModelIndex &index )
{
  auto load{ targetLoadsModelV->constLoad( targetLoadsModelV->load( index ) ) };

  if ( load )
  {
    emit activatedLoad( std::move( load ) );
  }
}

}
