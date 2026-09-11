// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::DirectoryWidget.
 **/

#include "DirectoryWidget.hpp"

#include "ui_DirectoryWidget.h"

#include <arinc_665_qt/media/MediaSetModel.hpp>

#include <arinc_665/media/Directory.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

DirectoryWidget::DirectoryWidget( QWidget * const parent ) :
  QWidget{ parent },
  uiV{ std::make_unique< Ui::DirectoryWidget >() }
{
  uiV->setupUi( this );

  uiV->content->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  connect(
    uiV->content,
    &QTableView::activated,
    this,
    &DirectoryWidget::selectElement );
}

DirectoryWidget::~DirectoryWidget() = default;

void DirectoryWidget::mediaSetModel( MediaSetModel * const model )
{
  mediaSetModelV = model;
  uiV->content->setModel( model );
}

void DirectoryWidget::selectDirectory( const QModelIndex &index )
{
  uiV->content->setRootIndex( index );
}

void DirectoryWidget::selectDirectory( Arinc665::Media::ConstDirectoryPtr directory )
{
  directoryV = std::move( directory );

  if ( directoryV )
  {
    uiV->nameLineEdit->setText( HelperQt::toQString( directoryV->name() ) );

    uiV->defaultMediumNumberGroupBox->setChecked( directoryV->defaultMediumNumber().has_value() );
    uiV->defaultMediumNumber->setValue( static_cast< uint8_t >( directoryV->effectiveDefaultMediumNumber() ) );
  }
}

void DirectoryWidget::selectElement( const QModelIndex &index )
{
  if ( auto element{ mediaSetModelV->element( index ) }; element )
  {
    emit activatedElement( std::move( element ) );
  }
}

}
