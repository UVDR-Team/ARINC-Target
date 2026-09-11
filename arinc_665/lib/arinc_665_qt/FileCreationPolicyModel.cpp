// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::FileCreationPolicyModel.
 **/

#include "FileCreationPolicyModel.hpp"

#include <arinc_665/utils/FileCreationPolicyDescription.hpp>

#include <helper_qt/String.hpp>

namespace Arinc665Qt {

FileCreationPolicyModel::FileCreationPolicyModel( QObject * const parent ) :
  QAbstractListModel{ parent }
{
}

FileCreationPolicyModel::~FileCreationPolicyModel() = default ;

int FileCreationPolicyModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.isValid() )
  {
    return 0;
  }

  return static_cast< int >(
    Arinc665::Utils::FileCreationPolicyDescription::instance()
      .descriptions().size() );
}

QVariant FileCreationPolicyModel::data( const QModelIndex &index, const int role ) const
{
  if ( !index.isValid() )
  {
    return {};
  }

  if ( std::cmp_greater_equal(
    index.row(),
    Arinc665::Utils::FileCreationPolicyDescription::instance().descriptions().size() ) )
  {
    return {};
  }

  // NOLINTNEXTLINE( readability-trivial-switch ): Keep for further extension
  switch ( role )
  {
    case Qt::ItemDataRole::DisplayRole:
      return HelperQt::toQString( Arinc665::Utils::FileCreationPolicyDescription::instance().name( index.row() ) );

    default:
      return {};
  }
}

std::optional< Arinc665::Utils::FileCreationPolicy > FileCreationPolicyModel::fileCreationPolicy(
  const QModelIndex &index ) const
{
  if ( !index.isValid()
    || std::cmp_greater_equal(
      index.row(),
      Arinc665::Utils::FileCreationPolicyDescription::instance()
        .descriptions().size() ) )
  {
    return {};
  }

  return Arinc665::Utils::FileCreationPolicyDescription::instance().enumeration(
    index.row() );
}

std::optional< Arinc665::Utils::FileCreationPolicy > FileCreationPolicyModel::fileCreationPolicy( const int row ) const
{
  if ( ( row < 0 )
    || std::cmp_greater_equal(
      row,
      Arinc665::Utils::FileCreationPolicyDescription::instance()
        .descriptions().size() ) )
  {
    return {};
  }

  return Arinc665::Utils::FileCreationPolicyDescription::instance().enumeration(
    row );
}

int FileCreationPolicyModel::fileCreationPolicy( Arinc665::Utils::FileCreationPolicy fileCreationPolicy ) const
{
  return Arinc665::Utils::FileCreationPolicyDescription::instance().value( fileCreationPolicy ).value_or( -1 );

}

}
