// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class HelperQt::VersionsModel.
 **/

#include "EndianModel.hpp"

#include <helper/EndianDescription.hpp>

namespace HelperQt {

std::optional< std::endian > EndianModel::endian( const int index )
{
  if ( index < 0 )
  {
    return {};
  }

  if ( std::cmp_greater_equal( index, Helper::EndianDescription::instance().descriptions().size() ) )
  {
    return {};
  }

  return std::next( Helper::EndianDescription::instance().descriptions().begin(), index )->enumeration;
}

std::optional< int > EndianModel::index( const std::endian endian )
{
  const auto &byEnumIndex{
    Helper::EndianDescription::instance().descriptions().get< Helper::EndianDescription::ByEnum >() };

  const auto pos{ byEnumIndex.find( endian ) };
  if ( Helper::EndianDescription::instance().end() == pos )
  {
    return {};
  }

  return static_cast< int >( std::distance( byEnumIndex.begin(), pos ) );
}

EndianModel::EndianModel( QObject * const parent ) :
  QAbstractListModel{ parent }
{
}

EndianModel::~EndianModel() = default;

int EndianModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Helper::EndianDescription::instance().descriptions().size() );
}

QVariant EndianModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  // out of range access
  if ( std::cmp_greater_equal( index.row(), Helper::EndianDescription::instance().descriptions().size() ) )
  {
    return {};
  }

  if ( 0 != index.column() )
  {
    return {};
  }

 const auto endianInformation{ std::next( Helper::EndianDescription::instance().begin(), index.row() ) };

  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
    case Qt::ItemDataRole::ToolTipRole:
      return QString::fromStdString( endianInformation->name );

    default:
      return {};
  }
}

QVariant EndianModel::headerData( const int section, const Qt::Orientation orientation, const int role ) const
{
  if ( role != Qt::DisplayRole )
  {
    return {};
  }

  if ( orientation == Qt::Vertical )
  {
    return section;
  }

  if ( 0 != section )
  {
    return {};
  }

  return QString{ tr( "Endian Order" ) };
}

}
