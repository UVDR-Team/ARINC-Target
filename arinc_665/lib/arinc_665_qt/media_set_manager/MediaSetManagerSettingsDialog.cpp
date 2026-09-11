// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::MediaSetManager::MediaSetManagerSettingsDialog.
 **/

#include "MediaSetManagerSettingsDialog.hpp"

#include "ui_MediaSetManagerSettingsDialog.h"

#include <arinc_665_qt/SupportedArinc665VersionModel.hpp>
#include <arinc_665_qt/FileCreationPolicyModel.hpp>

#include <arinc_665/utils/MediaSetDefaults.hpp>

#include <QSettings>

namespace Arinc665Qt::MediaSetManager {

MediaSetManagerSettingsDialog::MediaSetManagerSettingsDialog( QWidget * const parent ) :
  QDialog{ parent },
  uiV{ std::make_unique< Ui::MediaSetManagerSettingsDialog>() },
  supportedArinc665VersionModelV{
    std::make_unique< SupportedArinc665VersionModel >( this ) },
  fileCreationPolicyModelV{ std::make_unique< FileCreationPolicyModel >( this ) }
{
  uiV->setupUi( this );

  uiV->mediaSetDefaultsVersion->setModel( supportedArinc665VersionModelV.get() );
  uiV->mediaSetDefaultsLoadHeaderFilePolicy->setModel(
    fileCreationPolicyModelV.get() );
  uiV->mediaSetDefaultsBatchFilePolicy->setModel(
    fileCreationPolicyModelV.get() );
}

MediaSetManagerSettingsDialog::~MediaSetManagerSettingsDialog() = default;

Arinc665::Utils::MediaSetDefaults MediaSetManagerSettingsDialog::configuration() const
{
  Arinc665::Utils::MediaSetDefaults defaults{};

  defaults.checkFileIntegrity = uiV->mediaSetDefaultsCheckIntegrity->isChecked();
  defaults.version =
    supportedArinc665VersionModelV->supportedArinc665Version( uiV->mediaSetDefaultsVersion->currentIndex() )
      .value_or( Arinc665::Utils::MediaSetDefaults::DefaultVersion );
  defaults.loadHeaderFileCreationPolicy =
    fileCreationPolicyModelV->fileCreationPolicy( uiV->mediaSetDefaultsLoadHeaderFilePolicy->currentIndex() )
      .value_or( Arinc665::Utils::MediaSetDefaults::DefaultFileCreationPolicy );
  defaults.batchFileCreationPolicy =
    fileCreationPolicyModelV->fileCreationPolicy( uiV->mediaSetDefaultsBatchFilePolicy->currentIndex() )
      .value_or( Arinc665::Utils::MediaSetDefaults::DefaultFileCreationPolicy );

  QSettings settings{};
  settings.setValue(
    "CheckIntegrityOnStartup",
    uiV->checkIntegrityOnStartup->isChecked() );

  return defaults;
}

void MediaSetManagerSettingsDialog::configuration( const Arinc665::Utils::MediaSetDefaults &defaults )
{
  uiV->mediaSetDefaultsCheckIntegrity->setChecked( defaults.checkFileIntegrity );
  uiV->mediaSetDefaultsVersion->setCurrentIndex(
    supportedArinc665VersionModelV->supportedArinc665Version( defaults.version ) );
  uiV->mediaSetDefaultsLoadHeaderFilePolicy->setCurrentIndex(
    fileCreationPolicyModelV->fileCreationPolicy( defaults.loadHeaderFileCreationPolicy ) );
  uiV->mediaSetDefaultsBatchFilePolicy->setCurrentIndex(
    fileCreationPolicyModelV->fileCreationPolicy( defaults.batchFileCreationPolicy ) );

  QSettings settings{};
  uiV->checkIntegrityOnStartup->setChecked(
    settings.value( "CheckIntegrityOnStartup", true ).toBool() );
}

}
