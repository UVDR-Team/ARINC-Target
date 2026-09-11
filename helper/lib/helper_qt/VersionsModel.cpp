// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class HelperQt::VersionsModel.
 **/

#include "VersionsModel.hpp"

namespace HelperQt {

VersionsModel::VersionsModel( QObject * const parent ) :
  QAbstractTableModel{ parent }
{
}

VersionsModel::~VersionsModel() = default;

int VersionsModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( versionsInformationV.size() );
}

int VersionsModel::columnCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Columns::ColumnsCount );
}

QVariant VersionsModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // out of range access
  if ( std::cmp_greater_equal( index.row(), versionsInformationV.size() ) )
  {
    return {};
  }

  auto versionInformation{
    std::next( versionsInformationV.begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      switch ( Columns{ index.column() } )
      {
        case Columns::Name:
          return QString::fromStdString(
            versionInformation->second.productName );

        case Columns::Version:
          return QString::fromStdString(
            versionInformation->second.productVersion );

        case Columns::License:
          return QString::fromStdString(
            versionInformation->second.productLicense );

        case Columns::Url:
          return QString::fromStdString(
            versionInformation->second.productUrl );

        default:
          return {};
      }

    case Qt::ItemDataRole::ToolTipRole:
      if ( Columns::Url == Columns{ index.column() } )
      {
        return QString::fromStdString(
          versionInformation->second.productUrl );
      }
      return {};

    default:
      return {};
  }
}

QVariant VersionsModel::headerData( const int section, const Qt::Orientation orientation, const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  switch ( Columns{ section } )
  {
    case Columns::Name:
      return QString{ tr( "Name" ) };

    case Columns::Version:
      return QString{ tr( "Version" ) };

    case Columns::License:
      return QString{ tr( "License" ) };

    case Columns::Url:
      return QString{ tr( "URL" ) };

    default:
      return {};
  }
}

const Helper::VersionsInformation& VersionsModel::versions() const
{
  return versionsInformationV;
}

void VersionsModel::versions( Helper::VersionsInformation versions )
{
  beginResetModel();
  versionsInformationV = std::move( versions );
  endResetModel();
}

void VersionsModel::version( std::string_view key, Helper::VersionInformation information )
{
  beginResetModel();
  versionsInformationV.emplace( key, std::move( information ) );
  endResetModel();
}

Helper::VersionsInformation::value_type VersionsModel::version( const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal( index.row(), versionsInformationV.size() ) )
  {
    return {};
  }

  return *std::next( versionsInformationV.begin(), index.row() );
}

}
