// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Media::Directory.
 **/

#include "Directory.hpp"

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Media {

Directory::Directory(
  const ContainerEntityPtr &parent,
  std::string name,
  [[maybe_unused]] const CreateKey &createKey ) :
  nameV{ std::move( name ) },
  parentV{ parent }
{
  if ( !parent )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "parent must be valid" } );
  }
}

ConstMediaSetPtr Directory::mediaSet() const
{
  const auto parentPtr{ parent() };

  if ( !parentPtr )
  {
    return {};
  }

  return parentPtr->mediaSet();
}

MediaSetPtr Directory::mediaSet()
{
  const auto parentPtr{ parent() };

  if ( !parentPtr )
  {
    return {};
  }

  return parentPtr->mediaSet();
}

Type Directory::type() const
{
  return Type::Directory;
}

ContainerEntityPtr Directory::parent()
{
  return parentV.lock();
}

void Directory::parent( const ContainerEntityPtr &parent )
{
  if ( !parent )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{} << Helper::AdditionalInfo{ "parent must be valid" } );
  }

  if ( shared_from_this() == parent )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{} << Helper::AdditionalInfo{ "Recursion not allowed" } );
  }

  if ( this->parent() == parent )
  {
    return;
  }

  parentV = parent;
}

std::filesystem::path Directory::path() const
{
  if ( const auto parentPtr{ parent() }; parentPtr )
  {
    return parentPtr->path() / nameV;
  }

  return nameV;
}

std::string_view Directory::name() const
{
  return nameV;
}

void Directory::rename( std::string name )
{
  if ( const auto parentPtr{ parent() }; parentPtr )
  {
    if ( parentPtr->subdirectory( std::string_view( name ) ) || parentPtr->file( std::string_view( name ) ) )
    {
      BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
        << Helper::AdditionalInfo{ "directory or file with given names exist" }
        << boost::errinfo_file_name{ name } );
    }
  }

  nameV = std::move( name );
}

ConstContainerEntityPtr Directory::parent() const
{
  return parentV.lock();
}

}
