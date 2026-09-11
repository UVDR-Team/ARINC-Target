// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::LoadsModel.
 **/

#include "LoadsModel.hpp"

#include <arinc_665/media/Load.hpp>

#include <helper_qt/String.hpp>

#include <format>

namespace Arinc665Qt::Media {

LoadsModel::LoadsModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

LoadsModel::~LoadsModel() = default;

int LoadsModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( numberOfLoads() );
}

int LoadsModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant LoadsModel::data( const QModelIndex &index, const int role ) const
{
  auto loadPtr{ constLoad( load( index ) ) };

  // out of range access
  if ( !loadPtr )
  {
    return {};
  }

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( static_cast< Columns >( index.column() ) )
      {
        case Columns::Name:
          return HelperQt::toQString( loadPtr->name() );

        case Columns::PartNumber:
          return HelperQt::toQString( loadPtr->partNumber() );

        case Columns::LoadType:
          if ( auto loadType{ loadPtr->loadType() }; loadType )
          {
            return QString::fromStdString( std::format( "0x{:04x}: {:}", loadType->second, loadType->first ) );
          }
          return {};

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
      if ( const auto &loadType{ loadPtr->loadType() }; loadType )
      {
        return QString::fromStdString( loadType->first );
      }
      return {};

    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant LoadsModel::headerData( const int section, const ::Qt::Orientation orientation, const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    // Return section index as header
    return section;
  }

  switch ( static_cast< Columns >( section ) )
  {
    case Columns::Name:
      return QString{ tr( "Name" ) };

    case Columns::PartNumber:
      return QString{ tr( "Part Number" ) };

    case Columns::LoadType:
      return QString{ tr( "Load Type" ) };

    default:
      return {};
  }
}

size_t LoadsModel::numberOfLoads() const
{
  return std::visit(
    []( auto &loads ) { return loads.size(); },
    loadsV );
}

const Arinc665::Media::LoadsVariant& LoadsModel::loads() const
{
  return loadsV;
}

void LoadsModel::loads( Arinc665::Media::LoadsVariant loads )
{
  beginResetModel();
  loadsV = std::move( loads );
  endResetModel();
}

Arinc665::Media::ConstLoads LoadsModel::constLoads( const Arinc665::Media::LoadsVariant &loads ) const
{
  if ( holds_alternative< Arinc665::Media::ConstLoads >( loads ) )
  {
    return std::get< Arinc665::Media::ConstLoads >( loads );
  }

  const auto &realLoads{ std::get< Arinc665::Media::Loads >( loads ) };
  return Arinc665::Media::ConstLoads{ realLoads.begin(), realLoads.end() };
}

Arinc665::Media::LoadVariant LoadsModel::load( const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // return media set depending on variant
  return load( index.row() );
}

Arinc665::Media::LoadVariant LoadsModel::load( const std::size_t index ) const
{
  if ( index >= numberOfLoads() )
  {
    return {};
  }

  return std::visit(
    [ index ]( auto &loads ) {
      auto load{ std::next( loads.begin(), index ) };
      return Arinc665::Media::LoadVariant{ *load };
    },
    loadsV );
}

Arinc665::Media::ConstLoadPtr LoadsModel::constLoad( const Arinc665::Media::LoadVariant &load ) const
{
  return std::visit(
    []( const auto &load ) {
      return Arinc665::Media::ConstLoadPtr{ load };
    },
    load );
}

}
