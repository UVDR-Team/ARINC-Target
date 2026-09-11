// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::CompileMediaSetWizard.
 **/

#include "CompileMediaSetWizard.hpp"

#include "ui_CompileMediaSetWizard.h"

#include <arinc_665/utils/Arinc665Xml.hpp>
#include <arinc_665/utils/FilesystemMediaSetCompiler.hpp>
#include <arinc_665/utils/MediaSetDefaults.hpp>

#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <QMessageBox>

#include <boost/exception/all.hpp>

#include <cassert>

namespace Arinc665Qt {

CompileMediaSetWizard::CompileMediaSetWizard( QWidget * const parent ) :
  QWizard{ parent },
  ui{ std::make_unique< Ui::CompileMediaSetWizard >() },
  compilerV{ Arinc665::Utils::FilesystemMediaSetCompiler::create() }
{
  assert( compilerV );

  ui->setupUi( this );

  // set Logo of Wizard Pages
  QIcon icon{};
  icon.addFile(
    QString::fromUtf8( ":/fa/solid/gears.svg" ),
    QSize{},
    QIcon::Normal,
    QIcon::Off );
  for ( const auto pageId : pageIds() )
  {
    page( pageId )->setPixmap(
      QWizard::WizardPixmap::LogoPixmap,
      icon.pixmap( 64 ) );
  }

  connect(
    ui->settings,
    &CompileMediaSetSettingsPage::xmlFile,
    this,
    &CompileMediaSetWizard::xmlFile );
  connect(
    ui->settings,
    &CompileMediaSetSettingsPage::inputDirectory,
    this,
    &CompileMediaSetWizard::inputDirectory );
  connect(
    ui->settings,
    &CompileMediaSetSettingsPage::arinc665Version,
    this,
    &CompileMediaSetWizard::arinc665Version );
  connect(
    ui->settings,
    &CompileMediaSetSettingsPage::createBatchFiles,
    this,
    &CompileMediaSetWizard::createBatchFiles );
  connect(
    ui->settings,
    &CompileMediaSetSettingsPage::createLoadHeaderFiles,
    this,
    &CompileMediaSetWizard::createLoadHeaderFiles );
  connect(
    ui->settings,
    &CompileMediaSetSettingsPage::outputDirectory,
    this,
    &CompileMediaSetWizard::outputDirectory );

  connect(
    this,
    &QWizard::accepted,
    this,
    &CompileMediaSetWizard::compileMediaSet );

  // finally, set defaults (signals must be connected)
  ui->settings->defaults( Arinc665::Utils::MediaSetDefaults{} );
}

CompileMediaSetWizard::~CompileMediaSetWizard() = default;

void CompileMediaSetWizard::xmlFile( std::filesystem::path xmlFile )
{
  xmlFileV = std::move( xmlFile );
}

void CompileMediaSetWizard::inputDirectory( std::filesystem::path directory )
{
  compilerV->sourceBasePath( std::move( directory ) );
}

void CompileMediaSetWizard::arinc665Version( Arinc665::SupportedArinc665Version version )
{
  compilerV->arinc665Version( version );
}

void CompileMediaSetWizard::createBatchFiles( Arinc665::Utils::FileCreationPolicy createBatchFiles )
{
  compilerV->createBatchFiles( createBatchFiles );
}

void CompileMediaSetWizard::createLoadHeaderFiles( Arinc665::Utils::FileCreationPolicy createLoadHeaderFiles )
{
  compilerV->createLoadHeaderFiles( createLoadHeaderFiles );
}

void CompileMediaSetWizard::outputDirectory( std::filesystem::path directory )
{
  outputDirectoryV = std::move( directory );
}

void CompileMediaSetWizard::compileMediaSet()
{
  try
  {
    // load ARINC 665 XML file
    auto [ mediaSet, fileMapping ] = Arinc665::Utils::Arinc665Xml_load( xmlFileV );

    compilerV
      ->mediaSet( std::move( mediaSet ) )
      .filePathMapping( std::move( fileMapping ) )
      .outputBasePath( outputDirectoryV );
    assert( compilerV );
    auto [ mediaSetBasePath, mediaSetPaths ]{ ( *compilerV )() };

    QMessageBox::information(
      nullptr,
      tr( "Media Set Compilation successful" ),
      QString{ "Media Set created within <tt>%1</tt>" }
        .arg( QString::fromStdString( mediaSetBasePath.string() ) ) );
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during compilation" ),
      QString{ tr( "Error:<br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );
    return;
  }
  catch ( const std::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical(
      nullptr,
      tr( "Error during compilation" ),
      QString{ tr( "Error:<br/><tt>%1</tt>" ) }.arg( QString::fromStdString( info ) ) );
    return;
  }
}

}
