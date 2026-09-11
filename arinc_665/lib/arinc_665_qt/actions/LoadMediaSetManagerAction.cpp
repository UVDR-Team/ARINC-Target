// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Actions::LoadMediaSetManagerAction.
 **/

#include "LoadMediaSetManagerAction.hpp"

#include <arinc_665/utils/MediaSetManager.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper_qt/String.hpp>

#include <QMessageBox>
#include <QRunnable>
#include <QThreadPool>

#include <boost/exception/all.hpp>

namespace Arinc665Qt::Actions {

LoadMediaSetManagerAction::LoadMediaSetManagerAction( QObject * const parent ) :
  QObject{ parent }
{
  qRegisterMetaType< size_t >( "size_t" );
  qRegisterMetaType< Arinc665::Utils::MediaSetManagerPtr >( "Arinc665::Utils::MediaSetManagerPtr" );
}

LoadMediaSetManagerAction::~LoadMediaSetManagerAction()
{
}

void LoadMediaSetManagerAction::mediaSetDirectory( std::filesystem::path mediaSetDirectory )
{
  mediaSetDirectoryV = std::move( mediaSetDirectory );
}

void LoadMediaSetManagerAction::checkMediaSetIntegrity( const bool checkMediaSetIntegrity )
{
  checkMediaSetIntegrityV = checkMediaSetIntegrity;
}

void LoadMediaSetManagerAction::start()
{
  auto * const runnable{ QRunnable::create( [ this ] {
    try
    {
      const auto mediaSetManager{ Arinc665::Utils::MediaSetManager::loadOrCreate(
        mediaSetDirectoryV,
        checkMediaSetIntegrityV,
        std::bind_front( &LoadMediaSetManagerAction::loadProgress, this ) ) };

      emit mediaSetManagerLoaded( mediaSetManager );
    }
    catch ( const Arinc665::Arinc665Exception &e )
    {
      const auto info{ boost::diagnostic_information( e ) };

      emit failed( QString::fromStdString( info ) );
    }
    catch ( const std::exception &e )
    {
      const auto info{ boost::diagnostic_information( e ) };

      emit failed( QString::fromStdString( info ) );
    }
  } ) };

  QThreadPool::globalInstance()->start( runnable );
}

void LoadMediaSetManagerAction::loadProgress(
  const std::pair< std::size_t, std::size_t > mediaSet,
  const std::string_view partNumber,
  [[maybe_unused]] const std::pair< Arinc665::MediumNumber, Arinc665::MediumNumber > medium )
{
  emit mediaSetManagerLoadProgress( mediaSet.first, mediaSet.second, HelperQt::toQString( partNumber ) );
}

}
