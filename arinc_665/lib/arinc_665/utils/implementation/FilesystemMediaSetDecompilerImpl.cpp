// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::FilesystemMediaSetDecompilerImpl.
 **/

#include "FilesystemMediaSetDecompilerImpl.hpp"

#include <arinc_665/utils/MediaSetDecompiler.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/CheckValue.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <cassert>
#include <fstream>

namespace Arinc665::Utils {

FilesystemMediaSetDecompilerImpl::FilesystemMediaSetDecompilerImpl() :
  mediaSetDecompilerV{ MediaSetDecompiler::create() }
{
  assert( mediaSetDecompilerV );
  mediaSetDecompilerV
    ->fileSizeHandler( std::bind_front( &FilesystemMediaSetDecompilerImpl::getFileSize, this ) )
    .readFileHandler( std::bind_front( &FilesystemMediaSetDecompilerImpl::readFile, this ) );
}

FilesystemMediaSetDecompilerImpl::~FilesystemMediaSetDecompilerImpl() = default;

FilesystemMediaSetDecompiler& FilesystemMediaSetDecompilerImpl::progressHandler(
  MediaSetDecompiler::ProgressHandler progressHandler )
{
  assert( mediaSetDecompilerV );
  mediaSetDecompilerV->progressHandler( std::move( progressHandler ) );
  return *this;
}

FilesystemMediaSetDecompiler& FilesystemMediaSetDecompilerImpl::checkFileIntegrity(
  const bool checkFileIntegrity ) noexcept
{
  assert( mediaSetDecompilerV );
  mediaSetDecompilerV->checkFileIntegrity( checkFileIntegrity );
  return *this;
}

FilesystemMediaSetDecompiler &FilesystemMediaSetDecompilerImpl::mediaPaths( MediaPaths mediaPaths )
{
  mediaPathsV = std::move( mediaPaths );
  return *this;
}

MediaSetDecompilerResult FilesystemMediaSetDecompilerImpl::operator()()
{
  assert( mediaSetDecompilerV );
  return ( *mediaSetDecompilerV )();
}

size_t FilesystemMediaSetDecompilerImpl::getFileSize(
  const MediumNumber &mediumNumber,
  const std::filesystem::path &path )
{
  const auto mediumPath{ mediaPathsV.find( mediumNumber ) };

  if ( mediaPathsV.end() == mediumPath )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Medium not found" }
      << boost::errinfo_file_name{ path.string() } );
  }

  const auto filePath{ mediumPath->second / path.relative_path() };

  if ( !std::filesystem::is_regular_file( filePath ) )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  return std::filesystem::file_size( filePath );
}

Helper::RawData FilesystemMediaSetDecompilerImpl::readFile(
  const MediumNumber &mediumNumber,
  const std::filesystem::path &path )
{
  const auto mediumPath{ mediaPathsV.find( mediumNumber ) };

  if ( mediaPathsV.end() == mediumPath )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Medium not found" }
      << boost::errinfo_file_name{ path.string() } );
  }

  const auto filePath{ mediumPath->second / path.relative_path() };

  // check existence of the file
  if ( !std::filesystem::is_regular_file( filePath ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
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
