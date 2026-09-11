// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::MediaSetsModel.
 **/

#include "MediaSetsModel.hpp"

#include <arinc_665/media/MediaSet.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

MediaSetsModel::MediaSetsModel( QObject *const parent ) :
  QAbstractTableModel{ parent }
{
}

MediaSetsModel::~MediaSetsModel() = default;

int MediaSetsModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( numberOfMediaSets() );
}

int MediaSetsModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return std::to_underlying( Columns::ColumnsCount );
}

QVariant MediaSetsModel::data( const QModelIndex &index, const int role ) const
{
  const auto mediaSetPtr{ constMediaSet( mediaSet( index ) ) };

  if ( !mediaSetPtr )
  {
    return {};
  }

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
      switch ( static_cast< Columns >( index.column() ) )
      {
        case Columns::PartNumber:
          return HelperQt::toQString( mediaSetPtr->partNumber() );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant MediaSetsModel::headerData( const int section, const ::Qt::Orientation orientation, const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( static_cast< Columns >( section ) )
  {
    case Columns::PartNumber:
      return QString{ tr( "Part Number" ) };

    default:
      return {};
  }
}

size_t MediaSetsModel::numberOfMediaSets() const
{
  return std::visit( []( auto &mediaSets ) { return mediaSets.size(); }, mediaSetsV );
}

const Arinc665::Media::MediaSetsVariant& MediaSetsModel::mediaSets() const
{
  return mediaSetsV;
}

void MediaSetsModel::mediaSets( Arinc665::Media::MediaSetsVariant mediaSets )
{
  beginResetModel();
  mediaSetsV = std::move( mediaSets );
  endResetModel();
}

Arinc665::Media::MediaSetVariant MediaSetsModel::mediaSet( const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // return media set depending on variant
  return mediaSet( index.row() );
}

Arinc665::Media::MediaSetVariant MediaSetsModel::mediaSet( const std::size_t index ) const
{
  if ( index >= numberOfMediaSets() )
  {
    return {};
  }

  return std::visit(
    [ index ]( auto &mediaSets ) {
      auto mediaSet{ std::next( mediaSets.begin(), index ) };
      return Arinc665::Media::MediaSetVariant{ *mediaSet };
    },
    mediaSetsV );
}

Arinc665::Media::ConstMediaSetPtr MediaSetsModel::constMediaSet(
  const Arinc665::Media::MediaSetVariant &mediaSet ) const
{
  return std::visit(
    []( const auto &mediaSet ) {
      return Arinc665::Media::ConstMediaSetPtr{ mediaSet };
    },
    mediaSet );
}

}
