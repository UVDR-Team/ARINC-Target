// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::ImportMediaSetXmlSettingsWidget.
 **/

#include "ImportMediaSetXmlSettingsWidget.hpp"

#include "ui_ImportMediaSetXmlSettingsWidget.h"

#include <QSettings>
#include <QStandardPaths>

namespace Arinc665Qt {

ImportMediaSetXmlSettingsWidget::ImportMediaSetXmlSettingsWidget(
  QWidget * const parent ) :
  QGroupBox{ parent },
  uiV{ std::make_unique< Ui::ImportMediaSetXmlSettingsWidget >() },
  selectXmlFileDialogV{ std::make_unique< QFileDialog >( this ) },
  selectInputDirectoryDialogV{ std::make_unique< QFileDialog >( this ) }
{
  uiV->setupUi( this );

  QSettings settings;

  selectXmlFileDialogV->setWindowTitle( tr( "Select ARINC 665 Media Set XML" ) );
  selectXmlFileDialogV->setNameFilter(tr( "ARINC 665 Media Set XML (*.xml)" ) );
  selectXmlFileDialogV->setFileMode( QFileDialog::FileMode::ExistingFile );
  selectXmlFileDialogV->setDirectory(
    settings
      .value(
        "LastImportMediaSetXmlDirectory",
        QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ) )
      .toString() );

  connect(
    uiV->selectXmlFile,
    &QPushButton::clicked,
    selectXmlFileDialogV.get(),
    qOverload<>( &QDialog::open ) );
  connect(
    selectXmlFileDialogV.get(),
    &QFileDialog::fileSelected,
    this,
    &ImportMediaSetXmlSettingsWidget::xmlFileSelected );

  selectInputDirectoryDialogV->setWindowTitle( tr( "Select Input Base Directory" ) );
  selectInputDirectoryDialogV->setFileMode( QFileDialog::FileMode::Directory );
  selectInputDirectoryDialogV->setOptions( QFileDialog::Option::ShowDirsOnly );

  connect(
    uiV->selectInputDirectory,
    &QPushButton::clicked,
    selectInputDirectoryDialogV.get(),
    qOverload<>( &QDialog::open ) );
  connect(
    selectInputDirectoryDialogV.get(),
    &QFileDialog::fileSelected,
    this,
    &ImportMediaSetXmlSettingsWidget::inputDirectorySelected );
}

ImportMediaSetXmlSettingsWidget::~ImportMediaSetXmlSettingsWidget() = default;

bool ImportMediaSetXmlSettingsWidget::completed() const
{
  return !uiV->xmlFile->text().isEmpty() && !uiV->inputDirectory->text().isEmpty();
}

void ImportMediaSetXmlSettingsWidget::xmlFileSelected( const QString &file )
{
  uiV->xmlFile->setText( file );
  emit xmlFile( file.toStdString() );

  // if not already set, update input directory
  if ( uiV->inputDirectory->text().isEmpty() )
  {
    inputDirectorySelected( selectXmlFileDialogV->directory().path() );
    selectInputDirectoryDialogV->setDirectory( selectXmlFileDialogV->directory() );
  }

  QSettings settings;
  settings.setValue( "LastImportMediaSetXmlDirectory", selectXmlFileDialogV->directory().path() );
}

void ImportMediaSetXmlSettingsWidget::inputDirectorySelected( const QString &file )
{
  uiV->inputDirectory->setText( file );
  emit inputDirectory( file.toStdString() );
}

}
