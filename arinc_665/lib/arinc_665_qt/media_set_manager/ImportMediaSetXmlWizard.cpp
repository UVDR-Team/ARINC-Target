// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::MediaSetManager::ImportMediaSetXmlWizard.
 **/

#include "ImportMediaSetXmlWizard.hpp"

#include "ui_ImportMediaSetXmlWizard.h"

#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/utils/Arinc665Xml.hpp>
#include <arinc_665/utils/FilesystemMediaSetCompiler.hpp>
#include <arinc_665/utils/MediaSetManager.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <QMessageBox>

#include <boost/exception/all.hpp>

#include <cassert>

namespace Arinc665Qt::MediaSetManager {

ImportMediaSetXmlWizard::ImportMediaSetXmlWizard(
  Arinc665::Utils::MediaSetManagerPtr mediaSetManager,
  QWidget * const parent ) :
  QWizard{ parent },
  uiV{ std::make_unique< Ui::ImportMediaSetXmlWizard >() },
  mediaSetManagerV{ std::move( mediaSetManager ) },
  compilerV{ Arinc665::Utils::FilesystemMediaSetCompiler::create() }
{
  assert( compilerV );

  uiV->setupUi( this );

  // set Logo of Wizard Pages
  QIcon icon{};
  icon.addFile(
    QString::fromUtf8( ":/fa/solid/file-import.svg" ),
    QSize{},
    QIcon::Normal,
    QIcon::Off );
  for ( const auto pageId : pageIds() )
  {
    page( pageId )->setPixmap( QWizard::WizardPixmap::LogoPixmap, icon.pixmap( 64 ) );
  }

  connect(
    uiV->settings,
    &ImportMediaSetXmlSettingsPage::xmlFile,
    this,
    &ImportMediaSetXmlWizard::xmlFile );
  connect(
    uiV->settings,
    &ImportMediaSetXmlSettingsPage::inputDirectory,
    this,
    &ImportMediaSetXmlWizard::inputDirectory );
  connect(
    uiV->settings,
    &ImportMediaSetXmlSettingsPage::arinc665Version,
    this,
    &ImportMediaSetXmlWizard::arinc665Version );
  connect(
    uiV->settings,
    &ImportMediaSetXmlSettingsPage::createBatchFiles,
    this,
    &ImportMediaSetXmlWizard::createBatchFiles );
  connect(
    uiV->settings,
    &ImportMediaSetXmlSettingsPage::createLoadHeaderFiles,
    this,
    &ImportMediaSetXmlWizard::createLoadHeaderFiles );
  connect(
    uiV->settings,
    &ImportMediaSetXmlSettingsPage::checkFileIntegrity,
    this,
    &ImportMediaSetXmlWizard::checkFileIntegrity );

  connect(
    this,
    &QWizard::accepted,
    this,
    &ImportMediaSetXmlWizard::importMediaSetXml );

  // finally set defaults (signals must be connected)
  uiV->settings->defaults( mediaSetManagerV->mediaSetDefaults() );
}

ImportMediaSetXmlWizard::~ImportMediaSetXmlWizard() = default;

void ImportMediaSetXmlWizard::xmlFile( std::filesystem::path xmlFile )
{
  xmlFileV = std::move( xmlFile );
}

void ImportMediaSetXmlWizard::inputDirectory( std::filesystem::path directory )
{
  compilerV->sourceBasePath( std::move( directory ) );
}

void ImportMediaSetXmlWizard::arinc665Version( const Arinc665::SupportedArinc665Version version )
{
  compilerV->arinc665Version( version );
}

void ImportMediaSetXmlWizard::createBatchFiles( Arinc665::Utils::FileCreationPolicy createBatchFiles )
{
  compilerV->createBatchFiles( createBatchFiles );
}

void ImportMediaSetXmlWizard::createLoadHeaderFiles( Arinc665::Utils::FileCreationPolicy createLoadHeaderFiles )
{
  compilerV->createLoadHeaderFiles( createLoadHeaderFiles );
}

void ImportMediaSetXmlWizard::checkFileIntegrity( const bool checkFileIntegrity )
{
  checkFileIntegrityV = checkFileIntegrity;
}

void ImportMediaSetXmlWizard::importMediaSetXml()
{
  try
  {
    // load ARINC 665 XML file
    auto [ mediaSet, fileMapping ] =
      Arinc665::Utils::Arinc665Xml_load( xmlFileV );

    compilerV
      ->mediaSet( std::move( mediaSet ) )
      .filePathMapping( std::move( fileMapping ) )
      .outputBasePath( mediaSetManagerV->directory() );
    assert( compilerV );

    auto mediaSetPaths{ ( *compilerV )() };

    mediaSetManagerV->registerMediaSet( mediaSetPaths, checkFileIntegrityV );
    mediaSetManagerV->saveConfiguration();
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during compilation" ),
      QString{ tr( "Error:<br/><tt>%1</tt>") }
        .arg( QString::fromStdString( info ) ) );
    return;
  }
  catch ( const std::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during compilation" ),
      QString{ tr( "Error:<br/><tt>%1</tt>") }
        .arg( QString::fromStdString( info ) ) );
    return;
  }
}

}
