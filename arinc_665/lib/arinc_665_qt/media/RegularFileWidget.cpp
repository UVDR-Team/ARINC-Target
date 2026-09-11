// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::RegularFileWidget.
 **/

#include "RegularFileWidget.hpp"

#include "ui_RegularFileWidget.h"

#include <arinc_665_qt/media/LoadsModel.hpp>

#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/RegularFile.hpp>

namespace Arinc665Qt::Media {

RegularFileWidget::RegularFileWidget( QWidget * const parent ) :
  QGroupBox{ parent },
  uiV{ std::make_unique< Ui::RegularFileWidget>() },
  usedInLoadsModelV{ std::make_unique< LoadsModel >( this ) }
{
  uiV->setupUi( this );

  uiV->usedInLoads->setModel( usedInLoadsModelV.get() );

  uiV->usedInLoads->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );

  connect(
    uiV->usedInLoads,
    &QTableView::activated,
    this,
    &RegularFileWidget::activateLoad );
}

RegularFileWidget::~RegularFileWidget() = default;

void RegularFileWidget::selectFile( Arinc665::Media::ConstRegularFilePtr file )
{
  fileV = std::move( file );

  if ( fileV )
  {
    usedInLoadsModelV->loads( fileV->mediaSet()->loadsWithFile( fileV ) );
  }
}

void RegularFileWidget::activateLoad( const QModelIndex &index )
{
  if (  auto load{ usedInLoadsModelV->constLoad( usedInLoadsModelV->load( index ) ) }; load )
  {
    emit activatedLoad( std::move( load ) );
  }
}

}
