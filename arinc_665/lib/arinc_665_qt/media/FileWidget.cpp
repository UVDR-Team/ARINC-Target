// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::FileWidget.
 **/

#include "FileWidget.hpp"

#include "ui_FileWidget.h"

#include <arinc_665/media/Batch.hpp>
#include <arinc_665/media/File.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <arinc_649_qt/CheckValueTypeModel.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

FileWidget::FileWidget( QWidget * const parent ):
  QWidget{ parent },
  uiV{ std::make_unique< Ui::FileWidget >() },
  checkValueTypeModelV{ std::make_unique< Arinc649Qt::CheckValueTypeModel >( this ) }
{
  uiV->setupUi( this );

  uiV->checkValueType->setModel( checkValueTypeModelV.get() );

  connect(
    uiV->regularFilePage,
    &RegularFileWidget::activatedLoad,
    this,
    &FileWidget::activatedFile );
  connect(
    uiV->loadPage,
    &LoadWidget::activatedFile,
    this,
    &FileWidget::activatedFile );
  connect(
    uiV->loadPage,
    &LoadWidget::activatedBatch,
    this,
    &FileWidget::activatedFile );
  connect(
    uiV->batchPage,
    QOverload< Arinc665::Media::ConstLoadPtr >::of( &BatchWidget::activatedLoad ),
    this,
    &FileWidget::activatedFile );
}

FileWidget::~FileWidget() = default;

void FileWidget::selectFile( Arinc665::Media::ConstFilePtr file )
{
  fileV = std::move( file );

  if ( fileV )
  {
    uiV->nameLineEdit->setText( HelperQt::toQString( fileV->name() ) );

    uiV->mediumNumberGroupBox->setChecked( fileV->mediumNumber().has_value() );
    uiV->mediumNumber->setValue( static_cast< uint8_t >( fileV->effectiveMediumNumber() ) );

    uiV->checkValueTypeGroupBox->setChecked( fileV->checkValueType().has_value() );
    uiV->checkValueType->setCurrentIndex(
      Arinc649Qt::CheckValueTypeModel::checkValueType( fileV->effectiveCheckValueType() ) );
  }

  switch ( fileV->fileType() )
  {
    case Arinc665::Media::FileType::RegularFile:
      uiV->detailsStackedWidget->setCurrentIndex( static_cast< int >( FileStackedWidget::RegularFile ) );
      uiV->regularFilePage->selectFile( std::dynamic_pointer_cast< const Arinc665::Media::RegularFile >( fileV ) );
      break;

    case Arinc665::Media::FileType::LoadFile:
      uiV->detailsStackedWidget->setCurrentIndex( static_cast< int >( FileStackedWidget::LoadFile ) );
      uiV->loadPage->selectLoad( std::dynamic_pointer_cast< const Arinc665::Media::Load >( fileV ) );
      break;

    case Arinc665::Media::FileType::BatchFile:
      uiV->detailsStackedWidget->setCurrentIndex( static_cast< int >( FileStackedWidget::BatchFile ) );
      uiV->batchPage->selectBatch( std::dynamic_pointer_cast< const Arinc665::Media::Batch >( fileV ) );
      break;

    default:
      break;
  }

}

}
