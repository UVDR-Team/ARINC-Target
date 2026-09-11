// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::ExportMediaSetSettingsWidget.
 **/

#include "ExportMediaSetSettingsWidget.hpp"

#include "ui_ExportMediaSetSettingsWidget.h"

#include <arinc_665_qt/SupportedArinc665VersionModel.hpp>
#include <arinc_665_qt/FileCreationPolicyModel.hpp>

#include <arinc_665/utils/MediaSetDefaults.hpp>

namespace Arinc665Qt {

ExportMediaSetSettingsWidget::ExportMediaSetSettingsWidget(
  QWidget * const parent ) :
  QGroupBox{ parent },
  ui{ std::make_unique< Ui::ExportMediaSetSettingsWidget >() },
  supportedArinc665VersionModelV{ std::make_unique< SupportedArinc665VersionModel >( this ) },
  fileCreationPolicyModelV{ std::make_unique< FileCreationPolicyModel >( this ) }
{
  ui->setupUi( this );

  connect(
    ui->arinc665Version,
    QOverload< int >::of( &QComboBox::currentIndexChanged ),
    this,
    &ExportMediaSetSettingsWidget::arinc665VersionIndexSelected );
  ui->arinc665Version->setModel( supportedArinc665VersionModelV.get() );

  connect(
    ui->batchFileCreation,
    QOverload< int >::of( &QComboBox::currentIndexChanged ),
    this,
    &ExportMediaSetSettingsWidget::createBatchFilesIndexSelected );
  connect(
    ui->loadHeaderCreation,
    QOverload< int >::of( &QComboBox::currentIndexChanged ),
    this,
    &ExportMediaSetSettingsWidget::createLoadHeadersFilesIndexSelected );

  ui->batchFileCreation->setModel( fileCreationPolicyModelV.get() );
  ui->loadHeaderCreation->setModel( fileCreationPolicyModelV.get() );
}

ExportMediaSetSettingsWidget::~ExportMediaSetSettingsWidget() = default;

bool ExportMediaSetSettingsWidget::completed() const
{
  return true;
}

void ExportMediaSetSettingsWidget::defaults( const Arinc665::Utils::MediaSetDefaults &defaults )
{
  ui->arinc665Version->setCurrentIndex( supportedArinc665VersionModelV->supportedArinc665Version( defaults.version ) );
  ui->loadHeaderCreation->setCurrentIndex(
    fileCreationPolicyModelV->fileCreationPolicy( defaults.loadHeaderFileCreationPolicy ) );
  ui->batchFileCreation->setCurrentIndex(
    fileCreationPolicyModelV->fileCreationPolicy( defaults.batchFileCreationPolicy ) );

  emit arinc665Version( defaults.version );
  emit createLoadHeaderFiles( defaults.loadHeaderFileCreationPolicy );
  emit createBatchFiles( defaults.batchFileCreationPolicy );
}

void ExportMediaSetSettingsWidget::arinc665VersionIndexSelected( int index )
{
  if ( const auto version{ supportedArinc665VersionModelV->supportedArinc665Version( index ) }; version )
  {
    emit arinc665Version( *version );
  }
}

void ExportMediaSetSettingsWidget::createBatchFilesIndexSelected( int index )
{
  if ( const auto policy{ fileCreationPolicyModelV->fileCreationPolicy( index ) }; policy )
  {
    emit createBatchFiles( *policy );
  }
}

void ExportMediaSetSettingsWidget::createLoadHeadersFilesIndexSelected( int index )
{
  if ( const auto policy{ fileCreationPolicyModelV->fileCreationPolicy( index ) }; policy )
  {
    emit createLoadHeaderFiles( *policy );
  }
}

}
