// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::DecompileMediaSetSettingsPage.
 **/

#include "DecompileMediaSetSettingsPage.hpp"

#include "ui_DecompileMediaSetSettingsPage.h"

#include <arinc_665/utils/MediaSetDefaults.hpp>

namespace Arinc665Qt {

DecompileMediaSetSettingsPage::DecompileMediaSetSettingsPage( QWidget * const parent ) :
  QWizardPage{ parent },
  ui{ std::make_unique< Ui::DecompileMediaSetSettingsPage >() }
{
  ui->setupUi( this );

  connect(
    ui->mediaPaths,
    &MediaPathsWidget::mediaPathsChanged,
    this,
    &DecompileMediaSetSettingsPage::mediaPathsChanged );
  connect(
    ui->mediaPaths,
    &MediaPathsWidget::mediaPathsChanged,
    this,
    &DecompileMediaSetSettingsPage::completeChanged );

  connect(
    ui->checkFileIntegrity,
    &QCheckBox::checkStateChanged,
    this,
    &DecompileMediaSetSettingsPage::checkFileIntegrityStateChanged );
}

DecompileMediaSetSettingsPage::~DecompileMediaSetSettingsPage() = default;

bool DecompileMediaSetSettingsPage::isComplete() const
{
  return QWizardPage::isComplete() && ui->mediaPaths->completed();
}

void DecompileMediaSetSettingsPage::initializePage()
{
  QWizardPage::initializePage();

  ui->mediaPaths->clear();
}

void DecompileMediaSetSettingsPage::defaults( const Arinc665::Utils::MediaSetDefaults &defaults )
{
  ui->checkFileIntegrity->setChecked( defaults.checkFileIntegrity );
}

void DecompileMediaSetSettingsPage::checkFileIntegrityStateChanged( const Qt::CheckState state )
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
