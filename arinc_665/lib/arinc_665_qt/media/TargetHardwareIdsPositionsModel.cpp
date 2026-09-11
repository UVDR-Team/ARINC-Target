// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::TargetHardwareIdsPositionsModel.
 **/

#include "TargetHardwareIdsPositionsModel.hpp"

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

TargetHardwareIdsPositionsModel::TargetHardwareIdsPositionsModel(
  QObject * const parent ) :
  QAbstractItemModel{ parent }
{
}

QModelIndex TargetHardwareIdsPositionsModel::index(
  const int row,
  const int column,
  const QModelIndex &parent ) const
{
  // if parent is invalid, it is the target hardware id
  if ( !parent.isValid() )
  {
    if ( std::cmp_greater_equal( row, thwIdsPositionsV.size() ) )
    {
      return {};
    }

    return createIndex( row, column );
  }

  auto thwId{ std::next( thwIdsPositionsV.begin(), parent.row() ) };

  if ( std::cmp_greater_equal( row, thwId->second.size() ) )
  {
    return {};
  }

  return createIndex( row, column, (void*)( &( *thwId ) ) );
}

QModelIndex TargetHardwareIdsPositionsModel::parent(
  const QModelIndex &index ) const
{
  // invalid index has no parent
  if ( !index.isValid() )
  {
    return {};
  }

  if ( nullptr == index.internalPointer() )
  {
    // When no pointer set -> is THW ID
    return {};
  }

  auto * const thwId{ static_cast< Arinc665::Media::Load::TargetHardwareIdPositions::pointer >( index.internalPointer() ) };

  if ( nullptr == thwId )
  {
    return {};
  }

  const auto pos{ thwIdsPositionsV.find( thwId->first ) };

  if ( pos == thwIdsPositionsV.end() )
  {
    return {};
  }

  const auto row{ std::distance( thwIdsPositionsV.begin(), pos ) };

  return createIndex( static_cast< int >( row ), 0 );
}

bool TargetHardwareIdsPositionsModel::hasChildren( const QModelIndex &parent ) const
{
  // First level (media set)
  if ( !parent.isValid() )
  {
    // return true if an element is assigned
    return !thwIdsPositionsV.empty();
  }

  if ( nullptr != parent.internalPointer() )
  {
    // Positions
    return false;
  }

  if ( std::cmp_greater_equal( parent.row(), thwIdsPositionsV.size() ) )
  {
    return false;
  }

  const auto thwId{ std::next( thwIdsPositionsV.begin(), parent.row() ) };

  return !thwId->second.empty();
}

int TargetHardwareIdsPositionsModel::rowCount( const QModelIndex &parent ) const
{
  // First level (Media Set)
  if ( !parent.isValid() )
  {
    return static_cast< int >( thwIdsPositionsV.size() );
  }

  if ( nullptr != parent.internalPointer() )
  {
    // Positions
    return 0;
  }

  if ( std::cmp_greater_equal( parent.row(), thwIdsPositionsV.size() ) )
  {
    return 0;
  }

  const auto thwId{ std::next( thwIdsPositionsV.begin(), parent.row() ) };

  return static_cast< int >( thwId->second.size() );
}

int TargetHardwareIdsPositionsModel::columnCount( const QModelIndex &parent [[maybe_unused]] ) const
{
  return static_cast< int>( Columns::Last );
}

QVariant TargetHardwareIdsPositionsModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( !index.parent().isValid() )
  {
    return dataThwId( index, role );
  }

  return dataPosition( index, role );
}

QVariant TargetHardwareIdsPositionsModel::headerData(
  const int section,
  const Qt::Orientation orientation,
  const int role ) const
{
  if ( orientation == Qt::Vertical )
  {
    return {};
  }

  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( Columns{ section } )
  {
    case Columns::ThwIdPosition:
      return tr( "Target Hardware ID / Position" );

    default:
      return {};
  }
}

void TargetHardwareIdsPositionsModel::targetHardwareIdsPositions(
  Arinc665::Media::Load::TargetHardwareIdPositions thwIdsPositions )
{
  beginResetModel();
  thwIdsPositionsV = std::move( thwIdsPositions );
  endResetModel();
}

QVariant TargetHardwareIdsPositionsModel::dataThwId( const QModelIndex &index, const int role ) const
{
  if ( std::cmp_greater_equal( index.row(), thwIdsPositionsV.size() ) )
  {
    return {};
  }

  const auto thwId{ std::next( thwIdsPositionsV.begin(), index.row() ) };

  switch ( static_cast< Qt::ItemDataRole >( role ) )
  {
    case Qt::EditRole:
    case Qt::DisplayRole:
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
      switch ( Columns{ index.column() } )
      {
        case Columns::ThwIdPosition:
          return QString::fromStdString( thwId->first );

        default:
          return {};
      }

    default:
      return {};
  }
}

QVariant TargetHardwareIdsPositionsModel::dataPosition( const QModelIndex &index, const int role ) const
{
  const auto *thwId{
    static_cast< Arinc665::Media::Load::TargetHardwareIdPositions::pointer >( index.internalPointer() ) };

  if ( nullptr == thwId )
  {
    return 0;
  }

  if ( std::cmp_greater_equal( index.row(), thwId->second.size() ) )
  {
    return {};
  }

  const auto position{ std::next( thwId->second.begin(), index.row() ) };

  switch ( static_cast< Qt::ItemDataRole >( role ) )
  {
    case Qt::EditRole:
    case Qt::DisplayRole:
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
      switch ( Columns{ index.column() } )
      {
        case Columns::ThwIdPosition:
          return QString::fromStdString( *position );

        default:
          return {};
      }

    default:
      return {};
  }
}

}
