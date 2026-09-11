// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::MediaSetViewWidget.
 **/

#include "MediaSetViewWidget.hpp"

#include "ui_MediaSetViewWidget.h"

#include <arinc_665_qt/media/MediaSetModel.hpp>

#include <arinc_665/media/Base.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/File.hpp>
#include <arinc_665/media/Load.hpp>

namespace Arinc665Qt::Media {

MediaSetViewWidget::MediaSetViewWidget( QWidget * const parent ):
  QWidget{ parent },
  uiV{ std::make_unique< Ui::MediaSetViewWidget >() }
{
  uiV->setupUi( this );

  connect(
    uiV->mediaSetWidget,
    &MediaSetWidget::activatedElement,
    this,
    &MediaSetViewWidget::activateElement );
  connect(
    uiV->directoryWidget,
    &DirectoryWidget::activatedElement,
    this,
    &MediaSetViewWidget::activateElement );
  connect(
    uiV->fileWidget,
    &FileWidget::activatedFile,
    this,
    &MediaSetViewWidget::activateElement );
}

MediaSetViewWidget::~MediaSetViewWidget() = default;

void MediaSetViewWidget::mediaSetModel( Media::MediaSetModel * const model )
{
  if ( mediaSetModelV != nullptr )
  {
    // TODO disconnect old model
  }

  mediaSetModelV = model;

  uiV->mediaSetTreeView->setModel( mediaSetModelV );

  uiV->mediaSetWidget->mediaSetModel( mediaSetModelV );
  uiV->directoryWidget->mediaSetModel( mediaSetModelV );

  if ( nullptr != mediaSetModelV )
  {
    connect(
      mediaSetModelV,
      &Media::MediaSetModel::modelReset,
      [ this ]{
        uiV->mediaSetTreeView->setCurrentIndex( mediaSetModelV->index( 0, 0 ) );
        uiV->mediaSetTreeView->setExpanded( mediaSetModelV->index( 0, 0 ), true );
        uiV->mediaSetTreeView->resizeColumnToContents( 0 );

        // initiate update
        uiV->mediaSetTreeView->setCurrentIndex( mediaSetModelV->index( 0, 0 ) );
      } );

    connect(
      uiV->mediaSetTreeView->selectionModel(),
      &QItemSelectionModel::currentChanged,
      this,
      &MediaSetViewWidget::selectElement );
  }
}

void MediaSetViewWidget::selectElement( const QModelIndex &index )
{
  const auto element{ mediaSetModelV->element( index ) };

  if ( !element )
  {
    return;
  }

  switch ( element->type() )
  {
    case Arinc665::Media::Type::MediaSet:
    {
      const auto mediaSet{ std::dynamic_pointer_cast< const Arinc665::Media::MediaSet>( element ) };

      uiV->detailsStackedWidget->setCurrentIndex( static_cast< int >( DetailsStackedWidget::MediaSet ) );
      uiV->mediaSetWidget->selectMediaSet( mediaSet );
      break;
    }

    case Arinc665::Media::Type::Directory:
      uiV->detailsStackedWidget->setCurrentIndex( static_cast< int >( DetailsStackedWidget::Directory ) );
      uiV->directoryWidget->selectDirectory( index );
      uiV->directoryWidget->selectDirectory( std::dynamic_pointer_cast< const Arinc665::Media::Directory>( element ) );
      break;

    case Arinc665::Media::Type::File:
      uiV->detailsStackedWidget->setCurrentIndex( static_cast< int >( DetailsStackedWidget::File ) );
      uiV->fileWidget->selectFile( std::dynamic_pointer_cast< const Arinc665::Media::File>( element ) );
      break;

    default:
      break;
  }
}

void MediaSetViewWidget::activateElement( Arinc665::Media::ConstBasePtr element )
{
  auto index{ mediaSetModelV->indexForElement( element ) };
  uiV->mediaSetTreeView->setCurrentIndex( index );
  uiV->mediaSetTreeView->scrollTo( index );
}

}
