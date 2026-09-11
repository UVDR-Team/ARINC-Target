// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::FilesystemMediaSetCompilerImpl.
 **/

#include "FilesystemMediaSetCompilerImpl.hpp"

#include <arinc_665/utils/MediaSetCompiler.hpp>

#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/File.hpp>
#include <arinc_665/media/MediaSet.hpp>

#include <arinc_665/Arinc665Exception.hpp>
#include <arinc_665/MediumNumber.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <cassert>
#include <fstream>
#include <format>

namespace Arinc665::Utils {

FilesystemMediaSetCompilerImpl::FilesystemMediaSetCompilerImpl() :
  mediaSetCompilerV{ MediaSetCompiler::create() }
{
  mediaSetCompilerV
    ->createMediumHandler( std::bind_front( &FilesystemMediaSetCompilerImpl::createMedium, this ) )
    .createDirectoryHandler( std::bind_front( &FilesystemMediaSetCompilerImpl::createDirectory, this ) )
    .checkFileExistenceHandler( std::bind_front( &FilesystemMediaSetCompilerImpl::checkFileExistence, this ) )
    .createFileHandler( std::bind_front( &FilesystemMediaSetCompilerImpl::createFile, this ) )
    .writeFileHandler( std::bind_front( &FilesystemMediaSetCompilerImpl::writeFile, this ) )
    .readFileHandler( std::bind_front( &FilesystemMediaSetCompilerImpl::readFile, this ) );
}

FilesystemMediaSetCompilerImpl::~FilesystemMediaSetCompilerImpl() = default;

FilesystemMediaSetCompiler& FilesystemMediaSetCompilerImpl::mediaSet( Media::ConstMediaSetPtr mediaSet )
{
  assert( mediaSetCompilerV );

  // set media paths
  mediaPathsV.clear();
  for ( MediumNumber mediumNumber{}; mediumNumber <= mediaSet->lastMediumNumber(); ++mediumNumber )
  {
    mediaPathsV.try_emplace( mediumNumber, std::format( "MEDIUM_{:03d}", static_cast< uint8_t >( mediumNumber ) ) );
  }

  // set media set name
  if ( mediaSetNameV.empty() )
  {
    mediaSetNameV = mediaSet->partNumber();
  }

  mediaSetCompilerV->mediaSet( std::move( mediaSet ) );
  return *this;
}

FilesystemMediaSetCompiler &FilesystemMediaSetCompilerImpl::arinc665Version( const SupportedArinc665Version version )
{
  assert( mediaSetCompilerV );
  mediaSetCompilerV->arinc665Version( version );
  return *this;
}

FilesystemMediaSetCompiler& FilesystemMediaSetCompilerImpl::createBatchFiles(
  const FileCreationPolicy createBatchFiles )
{
  assert( mediaSetCompilerV );
  mediaSetCompilerV->createBatchFiles( createBatchFiles );
  return *this;
}

FilesystemMediaSetCompiler& FilesystemMediaSetCompilerImpl::createLoadHeaderFiles(
  const FileCreationPolicy createLoadHeaderFiles )
{
  assert( mediaSetCompilerV );
  mediaSetCompilerV->createLoadHeaderFiles( createLoadHeaderFiles );
  return *this;
}

FilesystemMediaSetCompiler& FilesystemMediaSetCompilerImpl::sourceBasePath( std::filesystem::path sourceBasePath )
{
  sourceBasePathV = std::move( sourceBasePath );
  return *this;
}

FilesystemMediaSetCompiler& FilesystemMediaSetCompilerImpl::filePathMapping( FilePathMapping filePathMapping )
{
  filePathMappingV = std::move( filePathMapping );
  return *this;
}

FilesystemMediaSetCompiler& FilesystemMediaSetCompilerImpl::outputBasePath( std::filesystem::path outputBasePath )
{
  outputBasePathV = std::move( outputBasePath );
  return *this;
}

FilesystemMediaSetCompiler& FilesystemMediaSetCompilerImpl::mediaSetName( std::string mediaSetName )
{
  mediaSetNameV = std::move( mediaSetName );
  return *this;
}

MediaSetPaths FilesystemMediaSetCompilerImpl::operator()()
{
  if ( sourceBasePathV.empty() || filePathMappingV.empty() || outputBasePathV.empty() || mediaSetNameV.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
      << Helper::AdditionalInfo{ "Not all parameter provided" } );
  }

  mediaSetBaseDirectoryV = outputBasePathV / mediaSetNameV;

  std::error_code err{};
  std::filesystem::create_directories( mediaSetBaseDirectoryV, err );
  if ( err )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
      << Helper::AdditionalInfo{ err.message() }
      << boost::errinfo_file_name{ mediaSetBaseDirectoryV.string() } );
  }

  assert( mediaSetCompilerV );
  ( *mediaSetCompilerV )();
  return { mediaSetNameV, mediaPathsV };
}

std::filesystem::path FilesystemMediaSetCompilerImpl::mediumPath( const Arinc665::MediumNumber &mediumNumber ) const
{
  const auto mediumPath{ mediaPathsV.find( mediumNumber ) };

  if ( mediaPathsV.end() == mediumPath )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ std::format( "Medium {} not found", mediumNumber ) } );
  }

  return mediaSetBaseDirectoryV / mediumPath->second;
}

void FilesystemMediaSetCompilerImpl::createMedium( const MediumNumber &mediumNumber )
{
  const auto mPath{ mediumPath( mediumNumber ) };

  SPDLOG_TRACE( "Create medium directory '{}'", mPath.string() );

  std::filesystem::create_directory( mPath );
}

void FilesystemMediaSetCompilerImpl::createDirectory(
  const MediumNumber &mediumNumber,
  const Media::ConstDirectoryPtr &directory )
{
  auto directoryPath{ mediumPath( mediumNumber ) / directory->path().relative_path() };

  SPDLOG_TRACE(
    "Create directory [{}]:'{}' ('{}')",
    mediumNumber.toString(),
    directory->path().string(),
    directoryPath.string() );

  std::filesystem::create_directory( directoryPath );
}

bool FilesystemMediaSetCompilerImpl::checkFileExistence( const Media::ConstFilePtr &file )
{
  SPDLOG_TRACE( "check existence of [{}]:'{}'", file->effectiveMediumNumber(), file->path().string() );

  // search for the file
  const auto fileIt{ filePathMappingV.find( file ) };

  if ( fileIt == filePathMappingV.end() )
  {
    return false;
  }

  const auto filePath{ ( sourceBasePathV / fileIt->second ).lexically_normal() };

  SPDLOG_TRACE( "found at '{}'", filePath.string() );

  return std::filesystem::is_regular_file( filePath );
}

void FilesystemMediaSetCompilerImpl::createFile( const Media::ConstFilePtr &file )
{
  // search the file
  const auto fileIt{ filePathMappingV.find( file ) };

  if ( fileIt == filePathMappingV.end() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "file mapping not found" }
      << boost::errinfo_file_name{ std::string{ file->name() } } );
  }

  const auto sourceFilePath{ ( sourceBasePathV / fileIt->second ).lexically_normal() };
  const auto destinationFilePath{ mediumPath( file->effectiveMediumNumber() ) / file->path().relative_path() };

  SPDLOG_TRACE( "Copy file from '{}' to '{}'", sourceFilePath.string(), destinationFilePath.string() );

  // copy file
  std::filesystem::copy( sourceFilePath, destinationFilePath );
}

void FilesystemMediaSetCompilerImpl::writeFile(
  const MediumNumber &mediumNumber,
  const std::filesystem::path &path,
  Helper::ConstRawDataSpan file )
{
  const auto filePath{ mediumPath( mediumNumber ) / path.relative_path() };

  SPDLOG_TRACE( "Write file [{}]:'{}' ('{}')", mediumNumber, path.string(), filePath.string() );

  // check the existence of the file
  if ( std::filesystem::exists( filePath ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "File already exists" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  // save file
  std::ofstream fileStream( filePath.string(), std::ofstream::binary | std::ofstream::out | std::ofstream::trunc );

  if ( !fileStream.is_open() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Error opening files" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  // write the data to the buffer
  fileStream.write( reinterpret_cast< const char * >( file.data() ), static_cast< std::streamsize >( file.size() ) );
}

Helper::RawData FilesystemMediaSetCompilerImpl::readFile(
  const MediumNumber &mediumNumber,
  const std::filesystem::path &path )
{
  // check medium number
  auto filePath{ mediumPath( mediumNumber ) / path.relative_path() };

  SPDLOG_TRACE( "Read file [{}]:'{}' ('{}')", mediumNumber, path.string(), filePath.string() );

  // check the existence of the file
  if ( !std::filesystem::is_regular_file( filePath ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  Helper::RawData data( std::filesystem::file_size( filePath ) );

  // load file
  std::ifstream file{ filePath.string().c_str(), std::ifstream::binary | std::ifstream::in };

  if ( !file.is_open() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Error opening files" }
      << boost::errinfo_file_name{ filePath.string() } );
  }

  // read the data to the buffer
  file.read( reinterpret_cast< char * >( std::data( data ) ), static_cast< std::streamsize >( std::size( data ) ) );

  // return the buffer
  return data;
}

}
