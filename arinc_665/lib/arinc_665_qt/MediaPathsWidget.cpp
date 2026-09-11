// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::MediaPathsWidget.
 **/

#include "MediaPathsWidget.hpp"

#include "ui_MediaPathsWidget.h"

#include <arinc_665_qt/MediaPathsModel.hpp>

#include <QMessageBox>

namespace Arinc665Qt {

MediaPathsWidget::MediaPathsWidget(
  QWidget * const parent ) :
  QGroupBox{ parent },
  uiV{ std::make_unique< Ui::MediaPathsWidget >() },
  mediaPathsModelV{ std::make_unique< MediaPathsModel >( this ) },
  selectMediaPathDialogV{ std::make_unique< QFileDialog >( this ) }
{
  uiV->setupUi( this );

  uiV->mediaPaths->setModel( mediaPathsModelV.get() );
  uiV->mediaPaths->horizontalHeader()->setSectionResizeMode(
    QHeaderView::ResizeMode::ResizeToContents );

  uiV->remove->setEnabled( false );

  selectMediaPathDialogV->setWindowTitle( tr( "Select Medium Directory" ) );
  selectMediaPathDialogV->setFileMode( QFileDialog::FileMode::Directory );
  selectMediaPathDialogV->setOptions( QFileDialog::Option::ShowDirsOnly );

  connect(
    uiV->mediaPaths->selectionModel(),
    &QItemSelectionModel::currentChanged,
    this,
    &MediaPathsWidget::updateButtons );

  connect(
    uiV->add,
    &QPushButton::pressed,
    selectMediaPathDialogV.get(),
    qOverload<>( &QDialog::open ) );

  connect(
    uiV->remove,
    &QPushButton::pressed,
    this,
    &MediaPathsWidget::removeMediumDirectory );

  connect(
    selectMediaPathDialogV.get(),
    &QFileDialog::fileSelected,
    this,
    &MediaPathsWidget::mediumDirectorySelected );
}

MediaPathsWidget::~MediaPathsWidget() = default;

bool MediaPathsWidget::completed() const
{
  return mediaPathsModelV->complete();
}

void MediaPathsWidget::clear()
{
  mediaPathsModelV->clear();
}

void MediaPathsWidget::updateButtons( const QModelIndex &current )
{
  uiV->remove->setEnabled( current.isValid() );
}

void MediaPathsWidget::removeMediumDirectory()
{
  mediaPathsModelV->remove( uiV->mediaPaths->currentIndex() );

  emit mediaPathsChanged( mediaPathsModelV->mediaPaths() );

  uiV->mediaPaths->setCurrentIndex( QModelIndex{} );
}

void MediaPathsWidget::mediumDirectorySelected( const QString &file )
{
  if ( !mediaPathsModelV->mediumPath( file.toStdString() ) )
  {
    QMessageBox::warning(
      this,
      tr( "Add Medium" ),
      QString{ tr( "%1 is not a valid ARINC 665 medium." ) }.arg( file ) );
    return;
  }

  uiV->mediaPaths->resizeColumnsToContents();
  emit mediaPathsChanged( mediaPathsModelV->mediaPaths() );
}

}
