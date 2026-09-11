// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Media::File.
 **/

#include "File.hpp"

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_665/media/ContainerEntity.hpp>
#include <arinc_665/media/MediaSet.hpp>

#include <arinc_649/Filename.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Media {

ConstMediaSetPtr File::mediaSet() const
{
  return parent()->mediaSet();
}

MediaSetPtr File::mediaSet()
{
  return parent()->mediaSet();
}

Type File::type() const
{
  return Type::File;
}

ConstContainerEntityPtr File::parent() const
{
  auto parentPtr{ parentV.lock() };
  assert( parentPtr );
  return parentPtr;
}

ContainerEntityPtr File::parent()
{
  auto parentPtr{ parentV.lock() };
  assert( parentPtr );
  return parentPtr;
}

std::filesystem::path File::path() const
{
  return parent()->path() / nameV;
}

std::string_view File::name() const
{
  return nameV;
}

void File::rename( std::string name )
{
  if ( parent()->subdirectory( std::string_view( name ) ) || parent()->file( std::string_view( name ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "directory or file with given names exist" }
      << boost::errinfo_file_name{ name } );
  }

  nameV = std::move( name );
}

MediumNumber File::effectiveMediumNumber() const
{
  return mediumNumberV.value_or( parent()->effectiveDefaultMediumNumber() );
}

OptionalMediumNumber File::mediumNumber() const
{
  return mediumNumberV;
}

void File::mediumNumber( const OptionalMediumNumber &mediumNumber )
{
  mediumNumberV = mediumNumber;
}

Arinc649::CheckValueType File::effectiveCheckValueType() const
{
  return checkValueTypeV.value_or( mediaSet()->effectiveFilesCheckValueType() );
}

std::optional< Arinc649::CheckValueType > File::checkValueType() const
{
  return checkValueTypeV;
}

void File::checkValueType( std::optional< Arinc649::CheckValueType > type )
{
  checkValueTypeV = type;
}

File::File(
  const ContainerEntityPtr &parent,
  std::string name,
  const OptionalMediumNumber &mediumNumber ) :
  parentV{ parent },
  nameV{ std::move( name ) },
  mediumNumberV{ mediumNumber }
{
  if ( !parent || !Arinc649::Filename_check( nameV ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "parameter invalid" } );
  }
}

void File::parent( const ContainerEntityPtr &parent )
{
  if ( !parent || ( mediaSet() != parent->mediaSet() ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "parent not valid or not on same media set" } );
  }

  if ( this->parent() == parent )
  {
    return;
  }

  parentV = parent;
}

}
