// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::FilePathMappingModel.
 **/

#include "FilePathMappingModel.hpp"

#include <arinc_665/media/File.hpp>

namespace Arinc665Qt {

FilePathMappingModel::FilePathMappingModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

FilePathMappingModel::~FilePathMappingModel() = default;

int FilePathMappingModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( filePathMappingV.size() );
}

int FilePathMappingModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant FilePathMappingModel::data( const QModelIndex &index, const int role ) const
{
  const auto mapping{ std::next( filePathMappingV.begin(), index.row() ) };
  if ( filePathMappingV.end() == mapping )
  {
    return {};
  }

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( static_cast< Columns>( index.column() ) )
      {
        case Columns::MediaSetFile:
          return QString::fromStdString( mapping->first->path().string() ) ;

        case Columns::FilePath:
          return QString::fromStdString( mapping->second.string() );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant FilePathMappingModel::headerData(
  const int section,
  const ::Qt::Orientation orientation,
  const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    // return index as header
    return section;
  }

  switch ( static_cast< Columns>( section ) )
  {
    case Columns::MediaSetFile:
      return QString{ tr( "File" ) };

    case Columns::FilePath:
      return QString{ tr( "Path" ) };

    default:
      return {};
  }
}

const Arinc665::Utils::FilePathMapping& FilePathMappingModel::filePathMapping() const
{
  return filePathMappingV;
}

void FilePathMappingModel::filePathMapping( Arinc665::Utils::FilePathMapping filePathMapping )
{
  beginResetModel();
  filePathMappingV = std::move( filePathMapping );
  endResetModel();
}

}
