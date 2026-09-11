// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::SupportedArinc665VersionModel.
 **/

#include "SupportedArinc665VersionModel.hpp"

#include <arinc_665/SupportedArinc665VersionDescription.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt {

SupportedArinc665VersionModel::SupportedArinc665VersionModel( QObject * const parent ) :
  QAbstractListModel{ parent }
{
}

SupportedArinc665VersionModel::~SupportedArinc665VersionModel() = default ;

int SupportedArinc665VersionModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >(
    Arinc665::SupportedArinc665VersionDescription::instance()
      .descriptions().size() );
}

QVariant SupportedArinc665VersionModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal(
    index.row(),
    Arinc665::SupportedArinc665VersionDescription::instance()
      .descriptions().size() ) )
  {
    return {};
  }

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      return HelperQt::toQString(
        Arinc665::SupportedArinc665VersionDescription::instance().name(
          index.row() ) );

    default:
      return {};
  }
}

std::optional< Arinc665::SupportedArinc665Version > SupportedArinc665VersionModel::supportedArinc665Version(
  const QModelIndex &index ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal(
    index.row(),
    Arinc665::SupportedArinc665VersionDescription::instance().descriptions().size() ) )
  {
    return {};
  }

  return Arinc665::SupportedArinc665VersionDescription::instance().enumeration( index.row() );
}

std::optional< Arinc665::SupportedArinc665Version > SupportedArinc665VersionModel::supportedArinc665Version(
  const int index ) const
{
  if ( std::cmp_greater_equal(
    index,
    Arinc665::SupportedArinc665VersionDescription::instance().descriptions().size() ) )
  {
    return {};
  }

  return Arinc665::SupportedArinc665VersionDescription::instance().enumeration( index );
}

int SupportedArinc665VersionModel::supportedArinc665Version( const Arinc665::SupportedArinc665Version version ) const
{
  return Arinc665::SupportedArinc665VersionDescription::instance().value( version ).value_or( -1 );
}

}
