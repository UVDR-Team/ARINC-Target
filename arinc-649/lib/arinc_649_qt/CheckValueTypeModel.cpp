// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc649Qt::CheckValueTypeModel.
 **/

#include "CheckValueTypeModel.hpp"

#include <arinc_649/CheckValueTypeDescription.hpp>

#include <helper_qt/String.hpp>

namespace Arinc649Qt {

std::optional< Arinc649::CheckValueType > CheckValueTypeModel::checkValueType( const QModelIndex &index )
{
  if ( !index.isValid() )
  {
    return std::nullopt;
  }

  if ( std::cmp_greater_equal( index.row(), Arinc649::CheckValueTypeDescription::instance().descriptions().size() ) )
  {
    return std::nullopt;
  }

  return Arinc649::CheckValueTypeDescription::instance().enumeration(
    static_cast< Arinc649::CheckValueTypeDescription::Value >( index.row() ) );
}

std::optional< Arinc649::CheckValueType > CheckValueTypeModel::checkValueType( const int index )
{
  if ( ( index < 0 )
    || std::cmp_greater_equal( index, Arinc649::CheckValueTypeDescription::instance().descriptions().size() ) )
  {
    return std::nullopt;
  }

  return Arinc649::CheckValueTypeDescription::instance().enumeration(
    static_cast< Arinc649::CheckValueTypeDescription::Value >( index ) );
}

int CheckValueTypeModel::checkValueType( const Arinc649::CheckValueType type )
{
  return Arinc649::CheckValueTypeDescription::instance().value( type ).value_or( -1 );
}

CheckValueTypeModel::CheckValueTypeModel( QObject * const parent ) :
  QAbstractListModel{ parent }
{
}

int CheckValueTypeModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >( Arinc649::CheckValueTypeDescription::instance().descriptions().size() );
}

QVariant CheckValueTypeModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal( index.row(), Arinc649::CheckValueTypeDescription::instance().descriptions().size() ) )
  {
    return {};
  }

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      return HelperQt::toQString(
        Arinc649::CheckValueTypeDescription::instance().name(
          static_cast< Arinc649::CheckValueTypeDescription::Value >( index.row() ) ) );

    default:
      return {};
  }
}

QVariant CheckValueTypeModel::headerData(
  const int section,
  const Qt::Orientation orientation,
  const int role ) const
{
  if ( orientation != Qt::Orientation::Horizontal )
  {
    return {};
  }

  if ( role != Qt::ItemDataRole::DisplayRole )
  {
    return {};
  }

  if ( 0 != section )
  {
    return {};
  }

  return QString{ tr( "Check Value Type" ) };
}

}
