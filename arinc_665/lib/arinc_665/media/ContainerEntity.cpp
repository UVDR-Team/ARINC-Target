// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Media::ContainerEntity.
 **/

#include "ContainerEntity.hpp"

#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/RegularFile.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/Batch.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <algorithm>
#include <cassert>

namespace Arinc665::Media {

MediumNumber ContainerEntity::effectiveDefaultMediumNumber() const
{
  if ( defaultMediumNumberV )
  {
    return *defaultMediumNumberV;
  }

  if ( const auto &parentV{ parent() }; parentV )
  {
    return parentV->effectiveDefaultMediumNumber();
  }

  return MediumNumber{ 1U };
}

OptionalMediumNumber ContainerEntity::defaultMediumNumber() const
{
  return defaultMediumNumberV;
}

void ContainerEntity::defaultMediumNumber( OptionalMediumNumber defaultMediumNumber )
{
  defaultMediumNumberV = defaultMediumNumber;
}

MediumNumber ContainerEntity::lastMediumNumber() const
{
  MediumNumber dirMax{ 1U };
  std::ranges::for_each(
    subdirectoriesV,
    [&dirMax]( const auto &dir )
    {
      dirMax = std::max( dirMax, dir->lastMediumNumber() );
    } );

  MediumNumber fileMax{ 1U };
  std::ranges::for_each(
    filesV,
    [&fileMax]( const auto &file  )
    {
      fileMax = std::max( fileMax, file->effectiveMediumNumber() );
    } );

  return std::max( dirMax, fileMax );
}

bool ContainerEntity::hasChildren( OptionalMediumNumber mediumNumber ) const
{
  if ( !mediumNumber )
  {
    return !subdirectoriesV.empty() || !filesV.empty();
  }

  const auto subdirsEmpty{
    std::ranges::all_of(
      subdirectoriesV,
      [ mediumNumber ]( const auto &subdir )
      {
        return subdir->hasChildren( mediumNumber );
      } ) };

  const auto filesEmpty{
    std::ranges::all_of(
      filesV,
      [ mediumNumber ]( const auto &file )
      {
        return file->effectiveMediumNumber() != mediumNumber;
      } ) };

  return !subdirsEmpty || !filesEmpty;
}

size_t ContainerEntity::numberOfSubdirectories( OptionalMediumNumber mediumNumber ) const
{
  if ( !mediumNumber )
  {
    return subdirectoriesV.size();
  }

  return std::ranges::count_if(
    subdirectoriesV,
    [ mediumNumber ]( const auto &subdir )
    {
      return subdir->hasChildren( mediumNumber );
    } );
}

ConstDirectories ContainerEntity::subdirectories( OptionalMediumNumber mediumNumber ) const
{
  if ( !mediumNumber )
  {
    return ConstDirectories{ subdirectoriesV.begin(), subdirectoriesV.end() };
  }

  ConstDirectories directories{};
  for ( const auto &subdirectory : subdirectoriesV )
  {
    if ( subdirectory->hasChildren( mediumNumber ) )
    {
      directories.emplace_back( subdirectory );
    }
  }

  return directories;
}

Directories ContainerEntity::subdirectories( const OptionalMediumNumber mediumNumber )
{
  if ( !mediumNumber )
  {
    return subdirectoriesV;
  }

  Directories directories{};
  for ( const auto &subdirectory : subdirectoriesV )
  {
    if ( subdirectory->hasChildren( mediumNumber ) )
    {
      directories.emplace_back( subdirectory );
    }
  }

  return directories;
}

ConstDirectoryPtr ContainerEntity::subdirectory( std::string_view name ) const
{
  for ( const auto &subdirectory : subdirectoriesV )
  {
    if ( subdirectory->name() == name )
    {
      return subdirectory;
    }
  }

  return {};
}

DirectoryPtr ContainerEntity::subdirectory( std::string_view name )
{
  for ( const auto &subdirectory : subdirectoriesV )
  {
    if ( subdirectory->name() == name )
    {
      return subdirectory;
    }
  }

  return {};
}

ConstContainerEntityPtr ContainerEntity::subdirectory( const std::filesystem::path &path ) const
{
  // normalise path
  const auto normalPath{ path.lexically_normal() };

  // handle empty path
  if ( normalPath.empty() )
  {
    return {};
  }

  // handle absolute paths
  // we cannot use 'is_absolute' under windows (other meaning)
  if ( normalPath.has_root_directory() )
  {
    const auto relativePath{ normalPath.relative_path() };

    if ( relativePath.empty() )
    {
      return mediaSet();
    }

    return mediaSet()->subdirectory( relativePath );
  }

  auto subDir{ std::dynamic_pointer_cast< const ContainerEntity >( shared_from_this() ) };
  assert( subDir );

  if ( normalPath == "." )
  {
    return subDir;
  }

  if ( normalPath == ".." )
  {
    return subDir->parent();
  }

  for ( const auto &subPath : normalPath )
  {
    subDir = subDir->subdirectory( std::string_view{ subPath.string() } );
    if ( !subDir )
    {
      return {};
    }
  }

  return subDir;
}

ContainerEntityPtr ContainerEntity::subdirectory( const std::filesystem::path &path )
{
  // normalise path
  const auto normalPath{ path.lexically_normal() };

  // handle empty path
  if ( normalPath.empty() )
  {
    return {};
  }

  // handle absolute paths
  // we cannot use 'is_absolute' under windows (other meaning)
  if ( normalPath.has_root_directory() )
  {
    const auto relativePath{ normalPath.relative_path() };

    if ( relativePath.empty() )
    {
      return mediaSet();
    }

    return mediaSet()->subdirectory( relativePath );
  }

  auto subDir{ std::dynamic_pointer_cast< ContainerEntity >( shared_from_this() ) };
  assert( subDir );

  for ( const auto &subPath : normalPath )
  {
    subDir = subDir->subdirectory( std::string_view{ subPath.string() } );
    if ( !subDir )
    {
      return {};
    }
  }

  return subDir;
}

DirectoryPtr ContainerEntity::addSubdirectory( std::string name )
{
  if ( ( "." == name ) || ( ".." == name ) )
  {
    return {};
  }

  if ( subdirectory( std::string_view( name ) ) || file( std::string_view( name ) ) )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665Exception{} << Helper::AdditionalInfo{ "subdirectory or file with name already exists" } );
  }

  // create, emplace and return directory
  return subdirectoriesV.emplace_back(
    std::make_shared< Directory >(
      std::dynamic_pointer_cast< ContainerEntity >( shared_from_this() ),
      std::move( name ),
      CreateKey{} ) );
}

void ContainerEntity::removeSubdirectory( std::string_view name )
{
  const auto dir{ std::ranges::find_if(
    subdirectoriesV,
    [ &name ]( const DirectoryPtr &dirEnt ){
      return ( name == dirEnt->name() );
    } ) };

  if ( subdirectoriesV.end() == dir )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{}
      << Helper::AdditionalInfo{ "subdirectory does not exists" }
      << boost::errinfo_file_name{ std::string{ name } } );
  }

  if ( (*dir)->hasChildren() )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{}
      << Helper::AdditionalInfo{ "subdirectory is not empty" }
      << boost::errinfo_file_name{ std::string{ name } } );
  }

  subdirectoriesV.erase( dir );
}

void ContainerEntity::removeSubdirectory( const DirectoryPtr &subDirectory )
{
  const auto dir{ std::ranges::find( subdirectoriesV, subDirectory ) };

  if ( subdirectoriesV.end() == dir )
   {
     BOOST_THROW_EXCEPTION( Arinc665Exception{}
       << Helper::AdditionalInfo{ "subdirectory does not exists" }
       << boost::errinfo_file_name{ std::string{ subDirectory->name() } } );
   }

   if ( (*dir)->hasChildren() )
   {
     BOOST_THROW_EXCEPTION( Arinc665Exception()
       << Helper::AdditionalInfo{ "subdirectory is not empty" }
       << boost::errinfo_file_name{ std::string{ subDirectory->name() } } );
   }

   subdirectoriesV.erase( dir );
}

size_t ContainerEntity::numberOfFiles( OptionalMediumNumber mediumNumber ) const
{
  if ( !mediumNumber )
  {
    return filesV.size();
  }

  return std::ranges::count_if(
    filesV,
    [&mediumNumber]( const auto &file )
    {
      return ( file->effectiveMediumNumber() == mediumNumber );
    } );
}

size_t ContainerEntity::recursiveNumberOfFiles( OptionalMediumNumber mediumNumber ) const
{
  size_t numberOfFilesRecursive{ numberOfFiles( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    numberOfFilesRecursive += subdirectory->recursiveNumberOfFiles( mediumNumber );
  }

  return numberOfFilesRecursive;
}

ConstFiles ContainerEntity::files( const OptionalMediumNumber mediumNumber ) const
{
  if ( !mediumNumber )
  {
    return ConstFiles{ filesV.begin(), filesV.end() };
  }

  ConstFiles files{};
  for ( const auto &file : filesV )
  {
    if ( mediumNumber == file->effectiveMediumNumber() )
    {
      files.emplace_back( file );
    }
  }

  return files;
}

Files ContainerEntity::files( const OptionalMediumNumber mediumNumber )
{
  if ( !mediumNumber )
  {
    return filesV;
  }

  Files files{};
  for ( const auto &file : filesV )
  {
    if ( mediumNumber == file->effectiveMediumNumber() )
    {
      files.emplace_back( file );
    }
  }

  return files;
}

ConstFiles ContainerEntity::recursiveFiles( OptionalMediumNumber mediumNumber ) const
{
  ConstFiles filesRecursive{ files( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    filesRecursive.splice( filesRecursive.begin(), subdirectory->recursiveFiles( mediumNumber ) );
  }

  return filesRecursive;
}

Files ContainerEntity::recursiveFiles( OptionalMediumNumber mediumNumber )
{
  Files filesRecursive{ files( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    filesRecursive.splice( filesRecursive.begin(), subdirectory->recursiveFiles( mediumNumber ) );
  }

  return filesRecursive;
}

ConstFiles ContainerEntity::recursiveFiles( std::string_view filename, OptionalMediumNumber mediumNumber ) const
{
  ConstFiles filesRecursive{};

  if ( auto fileFound{ file( filename ) }; fileFound )
  {
    // respect found file, when no medium number is provided or the medium
    // numbers are equal
    if ( !mediumNumber || mediumNumber == fileFound->effectiveMediumNumber() )
    {
      filesRecursive.emplace_back( std::move( fileFound ) );
    }
  }

  for ( const auto &subdirectory : subdirectories() )
  {
    filesRecursive.splice( filesRecursive.end(), subdirectory->recursiveFiles( filename, mediumNumber ) );
  }

  return filesRecursive;
}

Files ContainerEntity::recursiveFiles( std::string_view filename, OptionalMediumNumber mediumNumber )
{
  Files filesRecursive{};

  if ( auto fileFound{ file( filename ) }; fileFound )
  {
    // respect found file, when no medium number is provided or the medium
    // numbers are equal
    if ( !mediumNumber || mediumNumber == fileFound->effectiveMediumNumber() )
    {
      filesRecursive.emplace_back( std::move( fileFound ) );
    }
  }

  for ( const auto &subdirectory : subdirectories() )
  {
    filesRecursive.splice( filesRecursive.end(), subdirectory->recursiveFiles( filename, mediumNumber ) );
  }

  return filesRecursive;
}

ConstFilePtr ContainerEntity::file( const std::string_view filename ) const
{
  for ( const auto &file : filesV )
  {
    if ( file->name() == filename )
    {
      return file;
    }
  }

  return {};
}

FilePtr ContainerEntity::file( std::string_view filename )
{
  for ( const auto &file : filesV )
  {
    if ( file->name() == filename )
    {
      return file;
    }
  }

  return {};
}

ConstFilePtr ContainerEntity::file( const std::filesystem::path &path ) const
{
  if ( path.empty() )
  {
    return {};
  }

  auto dir{ std::dynamic_pointer_cast< const ContainerEntity >( shared_from_this() ) };
  assert( dir );
  if ( path.has_parent_path() )
  {
    dir = subdirectory( path.parent_path() );
  }

  return dir->file( std::string_view{ path.filename().string() } );
}

FilePtr ContainerEntity::file( const std::filesystem::path &path )
{
  if ( path.empty() )
  {
    return {};
  }

  auto dir{ std::dynamic_pointer_cast< ContainerEntity >( shared_from_this() ) };
  assert( dir );
  if ( path.has_parent_path() )
  {
    dir = subdirectory( path.parent_path() );
  }

  return dir->file( std::string_view{ path.filename().string() } );
}

void ContainerEntity::removeFile( std::string_view filename )
{
  const auto file{ std::ranges::find_if(
    filesV,
    [ &filename ]( const FilePtr &dirEnt ){
      return ( filename == dirEnt->name() );
    } ) };

  if ( filesV.end() == file )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ std::string{ filename } } );
  }

  // check when file is load, if it is part of batch
  if ( ( FileType::LoadFile == ( *file )->fileType() )
    && !mediaSet()->batchesWithLoad( std::dynamic_pointer_cast< const Load >( *file ) ).empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Load is part of Batch" }
      << boost::errinfo_file_name{ std::string{ filename } } );
  }

  // check when file is regular file, if it is part of load
  if ( ( FileType::RegularFile == ( *file )->fileType() )
    && !mediaSet()->loadsWithFile( std::dynamic_pointer_cast< const RegularFile >( *file ) ).empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Regular File is part of Load" }
      << boost::errinfo_file_name{ std::string{ filename } } );
  }

  filesV.erase( file );
}

void ContainerEntity::removeFile( const ConstFilePtr& file )
{
  const auto fileIt{ std::ranges::find( filesV, file ) };

  if ( filesV.end() == fileIt )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ std::string{ file->name() } } );
  }

  // check when file is load, if it is part of batch
  if ( ( FileType::LoadFile == (*fileIt)->fileType() )
    && !mediaSet()->batchesWithLoad( std::dynamic_pointer_cast< const Load >( *fileIt ) ).empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
      << Helper::AdditionalInfo{ "Load is part of Batch" }
      << boost::errinfo_file_name{ std::string{ file->name() } } );
  }

  // check when file is regular file, if it is part of load
  if ( ( FileType::RegularFile == (*fileIt)->fileType() )
    && !mediaSet()->loadsWithFile( std::dynamic_pointer_cast< const RegularFile >( *fileIt ) ).empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
      << Helper::AdditionalInfo{ "Regular File is part of Load" }
      << boost::errinfo_file_name{ std::string{ file->name() } } );
  }

  filesV.erase( fileIt );
}

size_t ContainerEntity::numberOfRegularFiles( OptionalMediumNumber mediumNumber ) const
{
  return numberOfFiles( FileType::RegularFile, mediumNumber );
}

size_t ContainerEntity::recursiveNumberOfRegularFiles( OptionalMediumNumber mediumNumber ) const
{
  size_t numberOfRegularFilesRecursive{ numberOfRegularFiles( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    numberOfRegularFilesRecursive += subdirectory->recursiveNumberOfRegularFiles( mediumNumber );
  }

  return numberOfRegularFilesRecursive;
}

ConstRegularFiles ContainerEntity::regularFiles( const OptionalMediumNumber mediumNumber ) const
{
  return filesPerType< ConstRegularFiles, FileType::RegularFile >( mediumNumber );
}

RegularFiles ContainerEntity::regularFiles( const OptionalMediumNumber mediumNumber )
{
  return filesPerType< RegularFiles, FileType::RegularFile >( mediumNumber );
}

ConstRegularFiles ContainerEntity::recursiveRegularFiles( const OptionalMediumNumber mediumNumber ) const
{
  ConstRegularFiles regularFilesRecursive{ regularFiles( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    regularFilesRecursive.splice( regularFilesRecursive.begin(), subdirectory->recursiveRegularFiles( mediumNumber ) );
  }

  return regularFilesRecursive;
}

RegularFiles ContainerEntity::recursiveRegularFiles( OptionalMediumNumber mediumNumber )
{
  RegularFiles regularFilesRecursive{ regularFiles( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    regularFilesRecursive.splice( regularFilesRecursive.begin(), subdirectory->recursiveRegularFiles( mediumNumber ) );
  }

  return regularFilesRecursive;
}

ConstRegularFiles ContainerEntity::recursiveRegularFiles(
  std::string_view filename,
  OptionalMediumNumber mediumNumber ) const
{
  ConstRegularFiles regularFiles{};

  // file within the current directory
  if ( auto foundRegularFile{ regularFile( filename ) }; foundRegularFile )
  {
    // respect found files, when no medium number is provided or the medium
    // numbers are equal
    if ( !mediumNumber || mediumNumber == foundRegularFile->effectiveMediumNumber() )
    {
      regularFiles.emplace_back( std::move( foundRegularFile ) );
    }
  }

  // files within subdirectories
  for ( const auto &subdirectory : subdirectories() )
  {
    regularFiles.splice( regularFiles.end(), subdirectory->recursiveRegularFiles( filename, mediumNumber ) );
  }

  return regularFiles;
}

RegularFiles ContainerEntity::recursiveRegularFiles(
  const std::string_view filename,
  const OptionalMediumNumber mediumNumber )
{
  RegularFiles regularFiles{};

  if ( auto foundRegularFile{ regularFile( filename ) }; foundRegularFile )
  {
    // respect found files, when no medium number is provided or the medium numbers are equal
    if ( !mediumNumber || mediumNumber == foundRegularFile->effectiveMediumNumber() )
    {
      regularFiles.emplace_back( std::move( foundRegularFile ) );
    }
  }

  for ( const auto &subdirectory : subdirectories() )
  {
    regularFiles.splice( regularFiles.end(), subdirectory->recursiveRegularFiles( filename, mediumNumber ) );
  }

  return regularFiles;
}

ConstRegularFilePtr ContainerEntity::regularFile( const std::string_view filename ) const
{
  return filePerType< ConstRegularFilePtr, FileType::RegularFile >( filename );
}

RegularFilePtr ContainerEntity::regularFile( const std::string_view filename )
{
  return filePerType< RegularFilePtr, FileType::RegularFile >( filename );
}

ConstRegularFilePtr ContainerEntity::regularFile( const std::filesystem::path &path ) const
{
  return filePerType< ConstRegularFilePtr, FileType::RegularFile >( path );
}

RegularFilePtr ContainerEntity::regularFile( const std::filesystem::path &path )
{
  return filePerType< RegularFilePtr, FileType::RegularFile >( path );
}

RegularFilePtr ContainerEntity::addRegularFile( std::string filename, OptionalMediumNumber mediumNumber )
{
  if ( ( "." == filename ) || ( ".." == filename ) )
  {
    return {};
  }

  if ( subdirectory( std::string_view( filename ) ) || file( std::string_view( filename ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "File or directory with name already exists" }
      << boost::errinfo_file_name{ filename } );
  }

  // create the file
  auto file{ std::make_shared< RegularFile >(
    std::dynamic_pointer_cast< ContainerEntity>( shared_from_this() ),
    std::move( filename ),
    mediumNumber,
    CreateKey{} ) };

  // create, emplace the file
  filesV.push_back( file );

  // return the new file
  return file;
}

size_t ContainerEntity::numberOfLoads( const OptionalMediumNumber mediumNumber ) const
{
  return numberOfFiles( FileType::LoadFile, mediumNumber );
}

size_t ContainerEntity::recursiveNumberOfLoads( const OptionalMediumNumber mediumNumber ) const
{
  size_t numberOfLoadsRecursive{ numberOfLoads( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    numberOfLoadsRecursive += subdirectory->recursiveNumberOfLoads( mediumNumber );
  }

  return numberOfLoadsRecursive;
}

ConstLoads ContainerEntity::loads( const OptionalMediumNumber mediumNumber ) const
{
  return filesPerType< ConstLoads, FileType::LoadFile>( mediumNumber );
}

Loads ContainerEntity::loads( const OptionalMediumNumber mediumNumber )
{
  return filesPerType< Loads, FileType::LoadFile>( mediumNumber );
}

ConstLoads ContainerEntity::recursiveLoads( const OptionalMediumNumber mediumNumber ) const
{
  ConstLoads loadsRecursive{ loads( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    loadsRecursive.splice( loadsRecursive.begin(), subdirectory->recursiveLoads( mediumNumber ) );
  }

  return loadsRecursive;
}

Loads ContainerEntity::recursiveLoads( OptionalMediumNumber mediumNumber )
{
  Loads loadsRecursive{ loads( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    loadsRecursive.splice( loadsRecursive.begin(), subdirectory->recursiveLoads( mediumNumber ) );
  }

  return loadsRecursive;
}

ConstLoads ContainerEntity::recursiveLoads(
  const std::string_view filename,
  const OptionalMediumNumber mediumNumber ) const
{
  ConstLoads loadsRecursive{};

  if ( auto foundLoad{ load( filename ) }; foundLoad )
  {
    // respect found load, when no medium number is provided or the medium
    // numbers are equal
    if ( !mediumNumber || mediumNumber == foundLoad->effectiveMediumNumber() )
    {
      loadsRecursive.emplace_back( std::move( foundLoad ) );
    }
  }

  for ( const auto &subdirectory : subdirectories() )
  {
    loadsRecursive.splice( loadsRecursive.end(), subdirectory->recursiveLoads( filename, mediumNumber ) );
  }

  return loadsRecursive;
}

Loads ContainerEntity::recursiveLoads( std::string_view filename, OptionalMediumNumber mediumNumber )
{
  Loads loadsRecursive{};

  if ( auto foundLoad{ load( filename ) }; foundLoad )
  {
    // respect found load, when no medium number is provided or the medium
    // numbers are equal
    if ( !mediumNumber || mediumNumber == foundLoad->effectiveMediumNumber() )
    {
      loadsRecursive.emplace_back( std::move( foundLoad ) );
    }
  }

  for ( const auto &subdirectory : subdirectories() )
  {
    loadsRecursive.splice( loadsRecursive.end(), subdirectory->recursiveLoads( filename, mediumNumber ) );
  }

  return loadsRecursive;
}

ConstLoadPtr ContainerEntity::load( std::string_view filename ) const
{
  return filePerType< ConstLoadPtr, FileType::LoadFile >( filename );
}

LoadPtr ContainerEntity::load( std::string_view filename )
{
  return filePerType< LoadPtr, FileType::LoadFile >( filename );
}

ConstLoadPtr ContainerEntity::load( const std::filesystem::path &path ) const
{
  return filePerType< ConstLoadPtr, FileType::LoadFile >( path );
}

LoadPtr ContainerEntity::load( const std::filesystem::path &path )
{
  return filePerType< LoadPtr, FileType::LoadFile >( path );
}

LoadPtr ContainerEntity::addLoad( std::string filename, OptionalMediumNumber mediumNumber )
{
  if ( ( "." == filename ) || ( ".." == filename ) )
  {
    return {};
  }

  if ( subdirectory( std::string_view( filename ) ) || file( std::string_view( filename ) ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{
        "File or directory with this name already exists" } );
  }

  // create the load
  auto load{ std::make_shared< Load >(
    std::dynamic_pointer_cast< ContainerEntity >( shared_from_this() ),
    std::move( filename ),
    mediumNumber,
    CreateKey{} ) };

  // insert into map
  filesV.push_back( load );

  // return the new load
  return load;
}

size_t ContainerEntity::numberOfBatches( const OptionalMediumNumber mediumNumber ) const
{
  return numberOfFiles( FileType::BatchFile, mediumNumber );
}

size_t ContainerEntity::recursiveNumberOfBatches( const OptionalMediumNumber mediumNumber ) const
{
  size_t numberOfBatchesRecursive{ numberOfBatches( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    numberOfBatchesRecursive += subdirectory->recursiveNumberOfBatches( mediumNumber );
  }

  return numberOfBatchesRecursive;
}

ConstBatches ContainerEntity::batches( const OptionalMediumNumber mediumNumber ) const
{
  return filesPerType< ConstBatches, FileType::BatchFile>( mediumNumber );
}

Batches ContainerEntity::batches( const OptionalMediumNumber mediumNumber )
{
  return filesPerType< Batches, FileType::BatchFile>( mediumNumber );
}

ConstBatches ContainerEntity::recursiveBatches( const OptionalMediumNumber mediumNumber ) const
{
  ConstBatches batchesRecursive{ batches( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    batchesRecursive.splice( batchesRecursive.begin(), subdirectory->recursiveBatches( mediumNumber ) );
  }

  return batchesRecursive;
}

Batches ContainerEntity::recursiveBatches( const OptionalMediumNumber mediumNumber )
{
  Batches batchesRecursive{ batches( mediumNumber ) };

  for ( const auto &subdirectory : subdirectories() )
  {
    batchesRecursive.splice( batchesRecursive.begin(), subdirectory->recursiveBatches( mediumNumber ) );
  }

  return batchesRecursive;
}

ConstBatches ContainerEntity::recursiveBatches(
  const std::string_view filename,
  const OptionalMediumNumber mediumNumber ) const
{
  ConstBatches batchesRecursive{};

  if ( auto foundBatch{ batch( filename ) }; foundBatch )
  {
    // respect found batch, when no medium number is provided, or the medium numbers are equal
    if ( !mediumNumber || mediumNumber == foundBatch->effectiveMediumNumber() )
    {
      batchesRecursive.emplace_back( std::move( foundBatch ) );
    }
  }

  for ( const auto &subdirectory : subdirectories() )
  {
    batchesRecursive.splice( batchesRecursive.end(), subdirectory->recursiveBatches( filename, mediumNumber ) );
  }

  return batchesRecursive;
}

Batches ContainerEntity::recursiveBatches( const std::string_view filename, const OptionalMediumNumber mediumNumber )
{
  Batches batchesRecursive{};

  if ( auto foundBatch{ batch( filename ) }; foundBatch )
  {
    // respect found batch, when no medium number is provided, or the medium numbers are equal
    if ( !mediumNumber || mediumNumber == foundBatch->effectiveMediumNumber() )
    {
      batchesRecursive.emplace_back( std::move( foundBatch ) );
    }
  }

  for ( const auto &subdirectory : subdirectories() )
  {
    batchesRecursive.splice( batchesRecursive.end(), subdirectory->recursiveBatches( filename, mediumNumber ) );
  }

  return batchesRecursive;
}

ConstBatchPtr ContainerEntity::batch( const std::string_view filename ) const
{
  return filePerType< ConstBatchPtr, FileType::BatchFile >( filename );
}

BatchPtr ContainerEntity::batch( const std::string_view filename )
{
  return filePerType< BatchPtr, FileType::BatchFile >( filename );
}

ConstBatchPtr ContainerEntity::batch( const std::filesystem::path &path ) const
{
  return filePerType< ConstBatchPtr, FileType::BatchFile >( path );
}

BatchPtr ContainerEntity::batch( const std::filesystem::path &path )
{
  return filePerType< BatchPtr, FileType::BatchFile >( path );
}

BatchPtr ContainerEntity::addBatch( std::string filename, OptionalMediumNumber mediumNumber )
{
  if ( ( "." == filename ) || ( ".." == filename ) )
  {
    return {};
  }

  if ( subdirectory( std::string_view( filename ) ) || file( std::string_view( filename ) ) )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665::Arinc665Exception{} << Helper::AdditionalInfo{ "File or directory with this name already exists" } );
  }

  // create the batch
  auto batch{ std::make_shared< Batch >(
    std::dynamic_pointer_cast< ContainerEntity >( shared_from_this() ),
    std::move( filename ),
    mediumNumber,
    CreateKey{} ) };

  // insert into map
  filesV.push_back( batch );

  // return the new batch
  return batch;
}

ContainerEntity::ContainerEntity( const OptionalMediumNumber defaultMediumNumber ) :
  defaultMediumNumberV{ defaultMediumNumber }
{
}

size_t ContainerEntity::numberOfFiles( const FileType fileType, const OptionalMediumNumber mediumNumber ) const
{
  return std::ranges::count_if(
    filesV,
    [fileType, &mediumNumber]( const auto &file )
    {
      return ( file->fileType() == fileType )
        && ( !mediumNumber || ( mediumNumber == file->effectiveMediumNumber() ) );
    } );
}

template< typename FilesT, FileType fileType >
FilesT ContainerEntity::filesPerType( const OptionalMediumNumber mediumNumber ) const
{
  FilesT result{};

  for ( const auto &file : filesV )
  {
    if ( ( file->fileType() == fileType ) && ( !mediumNumber || ( mediumNumber == file->effectiveMediumNumber() ) ) )
    {
      result.push_back( std::dynamic_pointer_cast< typename FilesT::value_type::element_type >( file ) );
    }
  }

  return result;
}

template< typename FilesT, FileType fileType >
FilesT ContainerEntity::filesPerType( const OptionalMediumNumber mediumNumber )
{
  FilesT result{};

  for ( const auto &file : filesV )
  {
    if ( ( file->fileType() == fileType ) && ( !mediumNumber || ( mediumNumber == file->effectiveMediumNumber() ) ) )
    {
      result.push_back( std::dynamic_pointer_cast< typename FilesT::value_type::element_type >( file ) );
    }
  }

  return result;
}

template< typename FilesT, FileType fileType >
FilesT ContainerEntity::filePerType( const std::string_view filename ) const
{
  const auto filePtr{ file( filename ) };

  if ( !filePtr )
  {
    return {};
  }

  if ( filePtr->fileType() != fileType )
  {
    return {};
  }

  return std::dynamic_pointer_cast< typename FilesT::element_type >( filePtr );
}

template< typename FilesT, FileType fileType >
FilesT ContainerEntity::filePerType( const std::string_view filename )
{
  const auto filePtr{ file( filename ) };

  if ( !filePtr )
  {
    return {};
  }

  if ( filePtr->fileType() != fileType )
  {
    return {};
  }

  return std::dynamic_pointer_cast< typename FilesT::element_type >( filePtr );
}

template< typename FilesT, FileType fileType >
FilesT ContainerEntity::filePerType( const std::filesystem::path &path ) const
{
  if ( path.empty() )
  {
    return {};
  }

  auto dir{ std::dynamic_pointer_cast< const ContainerEntity >( shared_from_this() ) };
  assert( dir );
  if ( path.has_parent_path() )
  {
    dir = subdirectory( path.parent_path() );
  }

  return dir->filePerType< FilesT, fileType >( std::string_view{ path.filename().string() } );
}

template< typename FilesT, FileType fileType >
FilesT ContainerEntity::filePerType( const std::filesystem::path &path )
{
  if ( path.empty() )
  {
    return {};
  }

  auto dir{ std::dynamic_pointer_cast< ContainerEntity >( shared_from_this() ) };
  assert( dir );
  if ( path.has_parent_path() )
  {
    dir = subdirectory( path.parent_path() );
    if ( !dir )
    {
      return {};
    }
  }

  return dir->filePerType< FilesT, fileType >( std::string_view{ path.filename().string() } );
}

}
