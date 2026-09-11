// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::Media::BatchInfoModel.
 **/

#include "BatchInfoModel.hpp"

#include <arinc_665/media/Batch.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt::Media {

BatchInfoModel::BatchInfoModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

BatchInfoModel::~BatchInfoModel() = default;

int BatchInfoModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( numberOfBatchInformation() );
}

int BatchInfoModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant BatchInfoModel::data( const QModelIndex &index, const int role ) const
{
  auto batchTargetInfo{ constBatchTargetInformation( batchTargetInformation( index ) ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
      switch ( static_cast< Columns>( index.column() ) )
      {
        case Columns::TargetHardwareIdPosition:
          return HelperQt::toQString( batchTargetInfo.first );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
    case Qt::ItemDataRole::TextAlignmentRole:
    default:
      return {};
  }
}

QVariant BatchInfoModel::headerData(
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
    // return section index as header
    return section;
  }

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( static_cast< Columns>( section ) )
  {
    case Columns::TargetHardwareIdPosition:
      return QString{ tr( "Target Hardware ID - Position" ) };

    default:
      return {};
  }
}

size_t BatchInfoModel::numberOfBatchInformation() const
{
  return std::visit(
    []( auto &batchInfo ) { return batchInfo.size(); },
    batchInformationV );
}

const Arinc665::Media::BatchInformationVariant& BatchInfoModel::batchInformation() const
{
  return batchInformationV;
}

void BatchInfoModel::batchInformation( Arinc665::Media::BatchInformationVariant information )
{
  beginResetModel();
  batchInformationV = std::move( information );
  endResetModel();
}

Arinc665::Media::BatchTargetInformationVariant  BatchInfoModel::batchTargetInformation( const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  return batchTargetInformation( index.row() );
}

Arinc665::Media::BatchTargetInformationVariant  BatchInfoModel::batchTargetInformation( const std::size_t index ) const
{
  if ( index >= numberOfBatchInformation() )
  {
    return {};
  }

  return std::visit(
    [ index ]( auto &batchInformation ) {
      auto batchTargetInformation{ std::next( batchInformation.begin(), index ) };
      return Arinc665::Media::BatchTargetInformationVariant{ *batchTargetInformation };
    },
    batchInformationV );
}

Arinc665::Media::ConstBatchTargetInformation BatchInfoModel::constBatchTargetInformation(
  const Arinc665::Media::BatchTargetInformationVariant &batchTargetInformation ) const
{
  return std::visit(
    []( const auto &batchTargetInformation ) {
      return Arinc665::Media::ConstBatchTargetInformation{
        batchTargetInformation.first,
        Arinc665::Media::ConstLoads{ batchTargetInformation.second.begin(), batchTargetInformation.second.end() } };
    },
    batchTargetInformation );
}

}
