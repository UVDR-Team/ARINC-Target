// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class HelperQt::AboutDialog.
 **/

#include "AboutDialog.hpp"

#include <ui_AboutDialog.h>

#include <QDesktopServices>
#include <QUrl>

namespace HelperQt {

AboutDialog::AboutDialog( QWidget * const parent ):
  QDialog{ parent },
  ui{ std::make_unique< Ui::AboutDialog >() },
  versionsModelV{ std::make_unique< VersionsModel >( this ) }
{
  ui->setupUi( this );

  ui->versions->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
  ui->versions->setModel( versionsModelV.get() );

  connect( ui->versions, &QTableView::doubleClicked, this, &AboutDialog::versionDoubleClicked );
}

AboutDialog::~AboutDialog() = default;

void AboutDialog::productLogo( const QPixmap &pixmap )
{
  ui->logo->setPixmap( pixmap );
}

void AboutDialog::productName( const QString &productName )
{
  ui->productName->setText( productName );
}

void AboutDialog::productVersion( const QString &productVersion )
{
  ui->productVersion->setText( productVersion );
}

void AboutDialog::productLicense( const QString &productLicense )
{
  ui->productLicense->setText( productLicense );
}

void AboutDialog::productUrl( const QString &productUrl )
{
  ui->productUrl->setText( QString{ tr( "<a href=\"%1\">%1</a>" ) }.arg( productUrl ) );
}

void AboutDialog::versions( Helper::VersionsInformation information )
{
  versionsModelV->versions( std::move( information ) );
}

void AboutDialog::versionDoubleClicked( const QModelIndex &index )
{
  if ( HelperQt::VersionsModel::Columns::Url == HelperQt::VersionsModel::Columns{ index.column() } )
  {
    QDesktopServices::openUrl( QUrl{ QString::fromStdString( versionsModelV->version( index ).second.productUrl ) } );
  }
}

}
