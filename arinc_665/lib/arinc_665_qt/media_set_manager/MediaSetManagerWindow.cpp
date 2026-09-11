// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::MediaSetManager::MediaSetManagerWindow.
 **/

#include "MediaSetManagerWindow.hpp"

#include "ui_MediaSetManagerWindow.h"

#include <arinc_665_qt/media_set_manager/ViewMediaSetDialog.hpp>
#include <arinc_665_qt/media_set_manager/ImportMediaSetWizard.hpp>
#include <arinc_665_qt/media_set_manager/ImportMediaSetXmlWizard.hpp>
#include <arinc_665_qt/media_set_manager/MediaSetManagerSettingsDialog.hpp>

#include <arinc_665_qt/media/MediaSetsModel.hpp>

#include <arinc_665_qt/actions/RemoveMediaSetAction.hpp>

#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/Version.hpp>

#include <arinc_649/Version.hpp>

#include <qt_icon_resources/Version.hpp>

#include <helper_qt/AboutDialog.hpp>
#include <helper_qt/String.hpp>

#include <helper/Version.hpp>

#include <QIcon>
#include <QDesktopServices>
#include <QUrl>

namespace Arinc665Qt::MediaSetManager {

MediaSetManagerWindow::MediaSetManagerWindow( QWidget * const parent ) :
  QMainWindow{ parent },
  uiV{ std::make_unique< Ui::MediaSetManagerWindow >() },
  viewMediaSetDialogV{ std::make_unique< ViewMediaSetDialog >( this ) },
  settingsDialogV{ std::make_unique< MediaSetManagerSettingsDialog >( this ) },
  aboutDialogV{ std::make_unique< HelperQt::AboutDialog >( this ) },
  mediaSetsModelV{ std::make_unique< Media::MediaSetsModel >( this ) }
{
  uiV->setupUi( this );

  uiV->mediaSets->setModel( mediaSetsModelV.get() );

  QIcon icon{};
  icon.addFile(
    QString::fromUtf8( ":/fa/solid/database.svg" ),
    QSize{},
    QIcon::Normal,
    QIcon::Off );
  aboutDialogV->productLogo( icon.pixmap( 64 ) );
  aboutDialogV->productName( QString{ "%1 (%2)" }.arg(
    tr( "ARINC 665 Media Set Manager" ),
    QString::fromStdString( Arinc665::Version::Name ) ) );
  aboutDialogV->productVersion(
    QString::fromStdString( Arinc665::Version::VersionInformation ) );
  aboutDialogV->productLicense(
    QString::fromStdString( Arinc665::Version::License ) );
  aboutDialogV->productUrl( QString::fromStdString( Arinc665::Version::Url ) );
  aboutDialogV->versions(
    {
      Arinc665::arinc665Version(),
      Arinc649::arinc649Version(),
      QtIconResources::qtIconResourcesVersion(),
      Helper::helperVersion(),
      Helper::boostVersion(),
      HelperQt::qtVersion()
    } );

  connect(
    uiV->mediaSets,
    &QTableView::activated,
    this,
    &MediaSetManagerWindow::viewMediaSet );

  connect(
    uiV->viewMediaSet,
    &QAction::triggered,
    this,
    &MediaSetManagerWindow::viewMediaSet );
  connect(
    uiV->importMediaSet,
    &QAction::triggered,
    this,
    &MediaSetManagerWindow::importMediaSet );
  connect(
    uiV->importMediaSetXml,
    &QAction::triggered,
    this,
    &MediaSetManagerWindow::importMediaSetXml );
  connect(
    uiV->removeMediaSet,
    &QAction::triggered,
    this,
    &MediaSetManagerWindow::removeMediaSet );
  connect(
    uiV->openMediaSetsDirectory,
    &QAction::triggered,
    this,
    &MediaSetManagerWindow::openMediaSetsDirectory );

  connect(
    uiV->mediaSetManagerSettings,
    &QAction::triggered,
    this,
    &MediaSetManagerWindow::showSettings );
  connect(
    settingsDialogV.get(),
    &MediaSetManagerSettingsDialog::accepted,
    this,
    &MediaSetManagerWindow::saveSettings );

  connect(
    uiV->about,
    &QAction::triggered,
    aboutDialogV.get(),
    QOverload<>::of( &HelperQt::AboutDialog::open ) );
}

MediaSetManagerWindow::~MediaSetManagerWindow() = default;

void MediaSetManagerWindow::mediaSetManger( Arinc665::Utils::MediaSetManagerPtr mediaSetManager )
{
  mediaSetManagerV = std::move( mediaSetManager );

  reloadMediaSetModel();
}

void MediaSetManagerWindow::reloadMediaSetModel()
{
  if ( !mediaSetsModelV )
  {
    return;
  }

  Arinc665::Media::ConstMediaSets mediaSets{};

  if ( mediaSetManagerV )
  {
    // convert media sets to const media sets
    std::ranges::transform(
      mediaSetManagerV->mediaSets(),
      std::back_inserter( mediaSets ),
      []( const auto &mediaSet )
      {
        return mediaSet.second.first;
      } );
  }

  mediaSetsModelV->mediaSets( std::move( mediaSets ) );

  uiV->mediaSets->selectRow( 0 );
}

void MediaSetManagerWindow::viewMediaSet()
{
  const auto index{ uiV->mediaSets->currentIndex() };

  if ( !index.isValid() )
  {
    return;
  }

  auto mediaSet{
    mediaSetsModelV->constMediaSet( mediaSetsModelV->mediaSet( index ) ) };

  if ( !mediaSet )
  {
    return;
  }

  viewMediaSetDialogV->setWindowTitle( HelperQt::toQString( mediaSet->partNumber() ) );
  viewMediaSetDialogV->mediaSet( std::move( mediaSet ) );
  viewMediaSetDialogV->show();
}

void MediaSetManagerWindow::importMediaSet()
{
  if ( !mediaSetsModelV )
  {
    return;
  }

  auto * const wizard{ new ImportMediaSetWizard{ mediaSetManagerV, this } };

  // connect to reload media set model slot
  connect(
    wizard,
    &ImportMediaSetWizard::finished,
    this,
    &MediaSetManagerWindow::reloadMediaSetModel );

  // connect to clean-up slot
  connect(
    wizard,
    &ImportMediaSetWizard::finished,
    wizard,
    &ImportMediaSetWizard::deleteLater );

  wizard->open();
}

void MediaSetManagerWindow::importMediaSetXml()
{
  if ( !mediaSetsModelV )
  {
    return;
  }

  auto * const wizard{ new ImportMediaSetXmlWizard{ mediaSetManagerV, this } };

  // connect to reload media set model slot
  connect(
    wizard,
    &ImportMediaSetXmlWizard::finished,
    this,
    &MediaSetManagerWindow::reloadMediaSetModel );

  // connect to clean-up slot
  connect(
    wizard,
    &ImportMediaSetXmlWizard::finished,
    wizard,
    &ImportMediaSetXmlWizard::deleteLater );

  wizard->open();
}

void MediaSetManagerWindow::removeMediaSet()
{
  const auto index{ uiV->mediaSets->currentIndex() };

  if ( !index.isValid() )
  {
    return;
  }

  const auto mediaSet{ mediaSetsModelV->constMediaSet( mediaSetsModelV->mediaSet( index ) ) };

  if ( !mediaSet )
  {
    return;
  }

  auto * const action{ new Actions::RemoveMediaSetAction{ this } };

  // connect to reload media set model slot
  connect(
    action,
    &Actions::RemoveMediaSetAction::finished,
    this,
    &MediaSetManagerWindow::reloadMediaSetModel );

  // connect to the clean-up slot
  connect(
    action,
    &Actions::RemoveMediaSetAction::finished,
    action,
    &Actions::RemoveMediaSetAction::deleteLater );

  action->start( mediaSetManagerV, mediaSet );
}

void MediaSetManagerWindow::openMediaSetsDirectory()
{
  if ( !mediaSetManagerV )
  {
    return;
  }

  QDesktopServices::openUrl( QUrl::fromLocalFile( QString::fromStdString( mediaSetManagerV->directory().string() ) ) );
}

void MediaSetManagerWindow::showSettings()
{
  settingsDialogV->configuration( mediaSetManagerV->mediaSetDefaults() );
  settingsDialogV->open();
}

void MediaSetManagerWindow::saveSettings()
{
  mediaSetManagerV->mediaSetDefaults( settingsDialogV->configuration() );
  mediaSetManagerV->saveConfiguration();
}

}
