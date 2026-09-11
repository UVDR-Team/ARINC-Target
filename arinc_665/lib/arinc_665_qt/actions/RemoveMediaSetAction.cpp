// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Actions::RemoveMediaSetAction.
 **/

#include "RemoveMediaSetAction.hpp"

#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/utils/FilesystemMediaSetRemover.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper_qt/String.hpp>

#include <QMessageBox>

#include <boost/exception/all.hpp>

#include <cassert>

namespace Arinc665Qt::Actions {

RemoveMediaSetAction::RemoveMediaSetAction( QWidget * const parent  ) :
  QObject{ parent }
{
}

RemoveMediaSetAction::~RemoveMediaSetAction() = default;

void RemoveMediaSetAction::start(
  const Arinc665::Utils::MediaSetManagerPtr &mediaSetManager,
  Arinc665::Media::ConstMediaSetPtr mediaSet )
{
  const auto answer{ QMessageBox::question(
    nullptr,
    tr( "Remove ARINC 665 Media Set" ),
    QString{ tr("Remove Media Set <i>%1</i>?" ) }
      .arg( HelperQt::toQString( mediaSet->partNumber() ) ) ) };

  switch ( answer )
  {
    case QMessageBox::StandardButton::Yes:
      break;

    case QMessageBox::StandardButton::No:
    default:
      emit finished();
      return;
  }

  try
  {
    auto mediaSetPaths{ mediaSetManager->deregisterMediaSet( mediaSet->partNumber() ) };
    mediaSetPaths.first = mediaSetManager->directory() / mediaSetPaths.first;

    const auto remover{ Arinc665::Utils::FilesystemMediaSetRemover::create() };
    assert( remover );

    remover->mediaSetPaths( mediaSetPaths );
    ( *remover )();
  }
  catch ( const Arinc665::Arinc665Exception &e )
  {
    const auto info{ boost::diagnostic_information( e ) };

    QMessageBox::critical( nullptr, tr( "Error during removal" ), QString::fromStdString( info ) );
  }

  mediaSetManager->saveConfiguration();

  emit finished();
}

}
