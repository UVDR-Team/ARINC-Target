// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::MediaSetViewerWindow.
 **/

#include "MediaSetViewerWindow.hpp"

#include "ui_MediaSetViewerWindow.h"

#include <arinc_665_qt/decompile_media_set/DecompileMediaSetWizard.hpp>

#include <arinc_665_qt/media/MediaSetModel.hpp>

#include <arinc_665_qt/FilePathMappingModel.hpp>

#include <arinc_665/utils/Arinc665Xml.hpp>
#include <arinc_665/utils/FilesystemMediaSetDecompiler.hpp>

#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/File.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/CheckValue.hpp>

#include <helper/Exception.hpp>

#include <helper_qt/String.hpp>

#include <spdlog/spdlog.h>

#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

#include <boost/exception/all.hpp>

namespace Arinc665Qt {

MediaSetViewerWindow::MediaSetViewerWindow( QWidget * const parent ):
  QMainWindow{ parent },
  ui{ std::make_unique< Ui::MediaSetViewerWindow >() },
  decompileMediaSetWizardV{ std::make_unique< DecompileMediaSetWizard >( this ) },
  selectLoadMediaSetXmlDialogV{ std::make_unique< QFileDialog>( this ) },
  selectSaveMediaSetXmlDialogV{ std::make_unique< QFileDialog>( this ) },
  mediaSetModelV{ std::make_unique< Media::MediaSetModel >( this ) },
  filePathMappingModelV{ std::make_unique< FilePathMappingModel >( this ) },
  sortedFilePathMappingModelV{ std::make_unique< QSortFilterProxyModel >( this ) },
  fileSystemWatcherV{ std::make_unique< QFileSystemWatcher>( this ) }
{
  QSettings settings{};

  ui->setupUi( this );

  ui->mediaSetView->mediaSetModel( mediaSetModelV.get() );
  sortedFilePathMappingModelV->setSourceModel( filePathMappingModelV.get() );
  sortedFilePathMappingModelV->sort(
    static_cast< int >( FilePathMappingModel::Columns::MediaSetFile ) );
  ui->filePathMapping->setModel( sortedFilePathMappingModelV.get() );

  connect(
    ui->actionDecompileMediaSet,
    &QAction::triggered,
    decompileMediaSetWizardV.get(),
    &QWizard::open );

  connect(
    decompileMediaSetWizardV.get(),
    &DecompileMediaSetWizard::mediaPathsChanged,
    this,
    &MediaSetViewerWindow::updateMediaPaths );
  connect(
    decompileMediaSetWizardV.get(),
    &DecompileMediaSetWizard::checkFileIntegrity,
    this,
    &MediaSetViewerWindow::checkFileIntegrity );
  connect(
    decompileMediaSetWizardV.get(),
    &DecompileMediaSetWizard::accepted,
    this,
    &MediaSetViewerWindow::startMediaSetDecompilation );


  selectLoadMediaSetXmlDialogV->setWindowTitle( tr( "Select ARINC 665 Media Set XML" ) );
  selectLoadMediaSetXmlDialogV->setNameFilter(tr( "ARINC 665 Media Set XML (*.xml)" ) );
  selectLoadMediaSetXmlDialogV->setFileMode( QFileDialog::FileMode::ExistingFile );
  selectLoadMediaSetXmlDialogV->setDirectory(
    settings
      .value(
        "LoadMediaSetXmlDirectory",
        QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ) )
      .toString() );

  connect(
    ui->actionOpenMediaSetXml,
    &QAction::triggered,
    selectLoadMediaSetXmlDialogV.get(),
    qOverload<>( &QDialog::open ) );
  connect(
    selectLoadMediaSetXmlDialogV.get(),
    &QFileDialog::fileSelected,
    this,
    &MediaSetViewerWindow::loadXmlFile );


  selectSaveMediaSetXmlDialogV->setWindowTitle( tr( "Select ARINC 665 Media Set XML" ) );
  selectSaveMediaSetXmlDialogV->setNameFilter(tr( "ARINC 665 Media Set XML (*.xml)" ) );
  selectSaveMediaSetXmlDialogV->setFileMode( QFileDialog::FileMode::AnyFile );
  selectSaveMediaSetXmlDialogV->setAcceptMode( QFileDialog::AcceptMode::AcceptSave );
  selectSaveMediaSetXmlDialogV->setDefaultSuffix( "xml" );
  selectSaveMediaSetXmlDialogV->setDirectory(
    settings
      .value(
        "SaveMediaSetXmlDirectory",
        QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ) )
      .toString() );

  connect(
    ui->actionSaveMediaSetXml,
    &QAction::triggered,
    selectSaveMediaSetXmlDialogV.get(),
    qOverload<>( &QDialog::open ) );
  connect(
    selectSaveMediaSetXmlDialogV.get(),
    &QFileDialog::fileSelected,
    this,
    &MediaSetViewerWindow::saveXmlFile );

  connect(
    fileSystemWatcherV.get(),
    &QFileSystemWatcher::fileChanged,
    this,
    &MediaSetViewerWindow::loadXmlFile );

  ui->actionSaveMediaSetXml->setEnabled( false );
}

MediaSetViewerWindow::~MediaSetViewerWindow() = default;

void MediaSetViewerWindow::updateMediaPaths(
  const Arinc665::Utils::MediaPaths &mediaPaths )
{
  mediaPathsV = mediaPaths;
}

void MediaSetViewerWindow::checkFileIntegrity( const bool checkFileIntegrity )
{
  checkFileIntegrityV = checkFileIntegrity;
}

void MediaSetViewerWindow::startMediaSetDecompilation()
{
  try
  {
    const auto decompiler{ Arinc665::Utils::FilesystemMediaSetDecompiler::create() };

    decompiler
      ->checkFileIntegrity( checkFileIntegrityV )
      .mediaPaths( mediaPathsV );

    auto [ mediaSet, checkValues ]{ ( *decompiler )() };

    Arinc665::Utils::FilePathMapping fileMapping{};

    // iterate over all files to add file-mapping
    for ( const auto &file : mediaSet->recursiveFiles() )
    {
      std::filesystem::path filePath( mediaPathsV.at( file->effectiveMediumNumber() ) / file->path().relative_path() );

      fileMapping.try_emplace( file, std::move( filePath ) );
    }

    const auto partNumber{ mediaSet->partNumber() };

    // remove all files from watching
    fileSystemWatcherV->removePaths( fileSystemWatcherV->files() );

    mediaSetModelV->root( std::move( mediaSet ) );
    filePathMappingModelV->filePathMapping( std::move( fileMapping ) );

    selectSaveMediaSetXmlDialogV->selectFile( HelperQt::toQString( partNumber ) + ".xml" );

    setWindowTitle( HelperQt::toQString( partNumber ) );

    ui->actionSaveMediaSetXml->setEnabled( true );
  }
  catch ( const boost::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical( nullptr, tr( "Error during decompilation" ), QString::fromStdString( info ) );
    return;
  }
  catch ( const std::exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical( nullptr, tr( "Error during decompilation" ), QString::fromStdString( info ) );
    return;
  }

  decompileMediaSetWizardV->restart();
}

void MediaSetViewerWindow::loadXmlFile( const QString &file )
{
  try
  {
    // remove all files from watching
    fileSystemWatcherV->removePaths( fileSystemWatcherV->files() );

    auto [ mediaSet, filePathMapping ]{ Arinc665::Utils::Arinc665Xml_load( file.toStdString() ) };

    mediaSetModelV->root( mediaSet );
    filePathMappingModelV->filePathMapping( std::move( filePathMapping ) );

    // Set window title
    setWindowTitle( HelperQt::toQString( mediaSet->partNumber() ) );

    ui->actionSaveMediaSetXml->setEnabled( true );

    selectSaveMediaSetXmlDialogV->selectFile( HelperQt::toQString( mediaSet->partNumber() ) + ".xml" );

    QSettings settings{};
    settings.setValue( "LoadMediaSetXmlDirectory", selectLoadMediaSetXmlDialogV->directory().path() );

    fileSystemWatcherV->addPath( file );
  }
  catch ( Arinc665::Arinc665Exception &e )
  {
    auto const * const info{ boost::get_error_info< Helper::AdditionalInfo >( e ) };

    QString description{};

    if ( nullptr == info )
    {
      description = e.what();
    }
    else
    {
      description = QString::fromStdString( *info );
    }

    SPDLOG_ERROR( boost::diagnostic_information( e, true ) );

    QMessageBox::critical( nullptr, tr( "Load Media Set XML" ), tr( "Error loading Media Set: " ) + description );
  }
}

void MediaSetViewerWindow::saveXmlFile( const QString &file )
{
  if ( !mediaSetModelV->root() )
  {
    return;
  }

  try
  {
    // export to ARINC 665 XML file
    Arinc665::Utils::Arinc665Xml_save(
      *mediaSetModelV->root()->mediaSet(),
      filePathMappingModelV->filePathMapping(),
      file.toStdString() );

    QSettings settings{};
    settings.setValue( "SaveMediaSetXmlDirectory", selectSaveMediaSetXmlDialogV->directory().path() );
  }
  catch ( const Arinc665::Arinc665Exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical( nullptr, tr( "Save Media Set XML" ), QString::fromStdString( info ) );
    return;
  }
}

}
