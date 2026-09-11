// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::CompileMediaSetSettingsPage.
 **/

#include "CompileMediaSetSettingsPage.hpp"

#include "ui_CompileMediaSetSettingsPage.h"

#include <QSettings>
#include <QStandardPaths>

namespace Arinc665Qt {

CompileMediaSetSettingsPage::CompileMediaSetSettingsPage( QWidget * const parent ) :
  QWizardPage{ parent },
  ui{ std::make_unique< Ui::CompileMediaSetSettingsPage >() },
  selectOutputDirectoryDialog{ std::make_unique< QFileDialog >( this ) }
{
  ui->setupUi( this );

  connect(
    ui->mediaSetXmlSettings,
    &ImportMediaSetXmlSettingsWidget::xmlFile,
    this,
    &CompileMediaSetSettingsPage::xmlFile );
  connect(
    ui->mediaSetXmlSettings,
    &ImportMediaSetXmlSettingsWidget::xmlFile,
    this,
    &QWizardPage::completeChanged );

  connect(
    ui->mediaSetXmlSettings,
    &ImportMediaSetXmlSettingsWidget::inputDirectory,
    this,
    &CompileMediaSetSettingsPage::inputDirectory );
  connect(
    ui->mediaSetXmlSettings,
    &ImportMediaSetXmlSettingsWidget::inputDirectory,
    this,
    &CompileMediaSetSettingsPage::completeChanged );

  connect(
    ui->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::arinc665Version,
    this,
    &CompileMediaSetSettingsPage::arinc665Version );
  connect(
    ui->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::arinc665Version,
    this,
    &CompileMediaSetSettingsPage::completeChanged );

  connect(
    ui->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::createBatchFiles,
    this,
    &CompileMediaSetSettingsPage::createBatchFiles );
  connect(
    ui->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::createBatchFiles,
    this,
    &CompileMediaSetSettingsPage::completeChanged );

  connect(
    ui->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::createLoadHeaderFiles,
    this,
    &CompileMediaSetSettingsPage::createLoadHeaderFiles );
  connect(
    ui->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::createLoadHeaderFiles,
    this,
    &CompileMediaSetSettingsPage::completeChanged );

  QSettings settings{};

  selectOutputDirectoryDialog->setWindowTitle( "Select Output Base Directory" );
  selectOutputDirectoryDialog->setFileMode( QFileDialog::FileMode::Directory );
  selectOutputDirectoryDialog->setOptions( QFileDialog::Option::ShowDirsOnly );
  selectOutputDirectoryDialog->setDirectory(
    settings
      .value(
        "LastOutputDirectory",
        QStandardPaths::standardLocations( QStandardPaths::AppLocalDataLocation ) )
      .toString() );

  connect(
    ui->selectOutputBasePath,
    &QPushButton::clicked,
    selectOutputDirectoryDialog.get(),
    qOverload<>( &QFileDialog::show ) );
  connect(
    selectOutputDirectoryDialog.get(),
    &QFileDialog::fileSelected,
    this,
    &CompileMediaSetSettingsPage::outputDirectorySelected );
}

CompileMediaSetSettingsPage::~CompileMediaSetSettingsPage() = default;

bool CompileMediaSetSettingsPage::isComplete() const
{
  return QWizardPage::isComplete()
    && ui->mediaSetXmlSettings->completed()
    && ui->mediaSetOutputSettings->completed()
    && !ui->outputBasePath->text().isEmpty();
}

void CompileMediaSetSettingsPage::defaults( const Arinc665::Utils::MediaSetDefaults &defaults )
{
  ui->mediaSetOutputSettings->defaults( defaults );
}

void CompileMediaSetSettingsPage::outputDirectorySelected( const QString &file )
{
  ui->outputBasePath->setText( file );

  emit outputDirectory( file.toStdString() );
  emit completeChanged();

  // store output path for next usage
  QSettings settings{};
  settings.setValue(
    "LastOutputDirectory",
    selectOutputDirectoryDialog->directory().path() );
}

}
