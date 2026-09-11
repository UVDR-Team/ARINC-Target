// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class
 *   Arinc665Qt::MediaSetManager::ImportMediaSetXmlSettingsPage.
 **/

#include "ImportMediaSetXmlSettingsPage.hpp"

#include "ui_ImportMediaSetXmlSettingsPage.h"

#include <arinc_665/utils/MediaSetDefaults.hpp>

namespace Arinc665Qt::MediaSetManager {

ImportMediaSetXmlSettingsPage::ImportMediaSetXmlSettingsPage( QWidget * const parent ) :
  QWizardPage{ parent },
  uiV{ std::make_unique< Ui::ImportMediaSetXmlSettingsPage >() }
{
  uiV->setupUi( this );

  connect(
    uiV->mediaSetXmlSettings,
    &ImportMediaSetXmlSettingsWidget::xmlFile,
    this,
    &ImportMediaSetXmlSettingsPage::xmlFile );
  connect(
    uiV->mediaSetXmlSettings,
    &ImportMediaSetXmlSettingsWidget::xmlFile,
    this,
    &QWizardPage::completeChanged );

  connect(
    uiV->mediaSetXmlSettings,
    &ImportMediaSetXmlSettingsWidget::inputDirectory,
    this,
    &ImportMediaSetXmlSettingsPage::inputDirectory );
  connect(
    uiV->mediaSetXmlSettings,
    &ImportMediaSetXmlSettingsWidget::inputDirectory,
    this,
    &ImportMediaSetXmlSettingsPage::completeChanged );

  connect(
    uiV->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::arinc665Version,
    this,
    &ImportMediaSetXmlSettingsPage::arinc665Version );
  connect(
    uiV->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::arinc665Version,
    this,
    &ImportMediaSetXmlSettingsPage::completeChanged );

  connect(
    uiV->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::createBatchFiles,
    this,
    &ImportMediaSetXmlSettingsPage::createBatchFiles );
  connect(
    uiV->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::createBatchFiles,
    this,
    &ImportMediaSetXmlSettingsPage::completeChanged );

  connect(
    uiV->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::createLoadHeaderFiles,
    this,
    &ImportMediaSetXmlSettingsPage::createLoadHeaderFiles );
  connect(
    uiV->mediaSetOutputSettings,
    &ExportMediaSetSettingsWidget::createLoadHeaderFiles,
    this,
    &ImportMediaSetXmlSettingsPage::completeChanged );

}

ImportMediaSetXmlSettingsPage::~ImportMediaSetXmlSettingsPage() = default;

bool ImportMediaSetXmlSettingsPage::isComplete() const
{
  return QWizardPage::isComplete()
    && uiV->mediaSetXmlSettings->completed()
    && uiV->mediaSetOutputSettings->completed();
}

void ImportMediaSetXmlSettingsPage::defaults( const Arinc665::Utils::MediaSetDefaults &defaults )
{
  uiV->mediaSetOutputSettings->defaults( defaults );
  uiV->checkFileIntegrity->setChecked( defaults.checkFileIntegrity );
}

void ImportMediaSetXmlSettingsPage::checkFileIntegrityStateChanged( const int state )
{
  switch ( state )
  {
    case Qt::CheckState::Unchecked:
      emit checkFileIntegrity( false );
      break;

    case Qt::CheckState::Checked:
      emit checkFileIntegrity( true );
      break;

    default:
      break;
  }
}

}
