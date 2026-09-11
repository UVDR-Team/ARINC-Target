// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::LoadFilesModel.
 **/

#include "LoadFilesModel.hpp"

#include <arinc_665/media/RegularFile.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

LoadFilesModel::LoadFilesModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

LoadFilesModel::~LoadFilesModel() = default;

int LoadFilesModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( loadFilesV.size() );
}

int LoadFilesModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant LoadFilesModel::data( const QModelIndex &index, const int role ) const
{
  auto loadFileInfo{ loadFile( index ) };

  if ( !loadFileInfo )
  {
    return {};
  }

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( static_cast< Columns>( index.column() ) )
      {
        case Columns::Name:
          return HelperQt::toQString( std::get< 0 >( *loadFileInfo )->name() );

        case Columns::PartNumber:
          return HelperQt::toQString( std::get< 1 >( *loadFileInfo ) );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant LoadFilesModel::headerData( const int section, const ::Qt::Orientation orientation, const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  switch ( static_cast< Columns>( section ) )
  {
    case Columns::Name:
      return QString{ tr( "Name" ) };

    case Columns::PartNumber:
      return QString{ tr( "Part Number" ) };

    default:
      return {};
  }
}

void LoadFilesModel::loadFiles( Arinc665::Media::ConstLoadFiles loadFiles )
{
  beginResetModel();
  loadFilesV = std::move( loadFiles );
  endResetModel();
}

std::optional< Arinc665::Media::ConstLoadFile > LoadFilesModel::loadFile( const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return std::nullopt;
  }

  return loadFile( index.row() );
}

std::optional< Arinc665::Media::ConstLoadFile > LoadFilesModel::loadFile( const size_t index ) const
{
  if ( std::cmp_greater_equal( index, loadFilesV.size() ) )
  {
    return std::nullopt;
  }

  return *std::next( loadFilesV.begin(), static_cast< std::ptrdiff_t >( index ) );
}

}
