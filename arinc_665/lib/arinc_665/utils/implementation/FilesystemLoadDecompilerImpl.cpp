// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::FilesystemLoadDecompilerImpl.
 **/

#include "FilesystemLoadDecompilerImpl.hpp"

#include <arinc_665/utils/LoadDecompiler.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <cassert>
#include <fstream>

namespace Arinc665::Utils {

FilesystemLoadDecompilerImpl::FilesystemLoadDecompilerImpl() :
  loadDecompilerV{ LoadDecompiler::create() }
{
  assert( loadDecompilerV );
  loadDecompilerV
    ->filePathHandler( std::bind_front( &FilesystemLoadDecompilerImpl::filePath, this ) )
    .fileSizeHandler( std::bind_front( &FilesystemLoadDecompilerImpl::fileSize, this ) )
    .readFileHandler( std::bind_front( &FilesystemLoadDecompilerImpl::readFile, this ) );
}

FilesystemLoadDecompiler& FilesystemLoadDecompilerImpl::checkFileIntegrity( bool checkFileIntegrity ) noexcept
{
  loadDecompilerV->checkFileIntegrity( checkFileIntegrity );
  return *this;
}

FilesystemLoadDecompiler& FilesystemLoadDecompilerImpl::loadPath( std::filesystem::path path )
{
  loadBasePathV = path.parent_path();
  loadDecompilerV->loadFilename( path.filename().string() );
  return *this;
}

LoadDecompilerResult FilesystemLoadDecompilerImpl::operator ()()
{
  return (*loadDecompilerV)();
}

std::filesystem::path FilesystemLoadDecompilerImpl::filePath( std::string_view filename ) const
{
  std::filesystem::path filePath;

  for ( auto entry : std::filesystem::recursive_directory_iterator{ loadBasePathV } )
  {
    if ( std::filesystem::is_regular_file( entry ) && entry.path().filename() == filename )
    {
      filePath = entry.path();
      break;
    }
  }

  if ( filePath.empty() )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665Exception{}
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ std::string{ filename } } );
  }

  return std::filesystem::relative( filePath, loadBasePathV );
}

size_t FilesystemLoadDecompilerImpl::fileSize( const std::filesystem::path &path )
{
  const auto filePath{ loadBasePathV / path };

  if ( !std::filesystem::is_regular_file( filePath ) )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  return std::filesystem::file_size( filePath );
}

Helper::RawData FilesystemLoadDecompilerImpl::readFile( const std::filesystem::path &path )
{
  const auto filePath{ loadBasePathV / path };

  // check existence of the file
  if ( !std::filesystem::is_regular_file( filePath ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  Helper::RawData data( std::filesystem::file_size( filePath ) );

  // load file
  std::ifstream file{ filePath, std::ifstream::binary | std::ifstream::in };

  if ( !file.is_open() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Error opening file" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  // read the data to the buffer
  file.read( reinterpret_cast< char * >( data.data() ), static_cast< std::streamsize >( data.size() ) );

  if ( file.bad() || ( file.gcount() != static_cast< std::streamsize >( data.size() ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Error reading file" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  // return the buffer
  return data;
}

}
