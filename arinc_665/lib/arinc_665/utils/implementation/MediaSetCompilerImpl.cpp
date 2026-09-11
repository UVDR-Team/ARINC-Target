// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::MediaSetCompilerImpl.
 **/

#include "MediaSetCompilerImpl.hpp"

#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/RegularFile.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/Batch.hpp>

#include <arinc_665/files/LoadListFile.hpp>
#include <arinc_665/files/BatchListFile.hpp>
#include <arinc_665/files/FileListFile.hpp>
#include <arinc_665/files/LoadHeaderFile.hpp>
#include <arinc_665/files/BatchFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/Arinc649Crc.hpp>
#include <arinc_649/CheckValueGenerator.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <utility>

namespace Arinc665::Utils {

MediaSetCompiler &MediaSetCompilerImpl::mediaSet( Media::ConstMediaSetPtr mediaSet )
{
  mediaSetV = std::move( mediaSet );
  return *this;
}

MediaSetCompiler &MediaSetCompilerImpl::createMediumHandler( CreateMediumHandler createMediumHandler )
{
  createMediumHandlerV = std::move( createMediumHandler );
  return *this;
}

MediaSetCompiler &MediaSetCompilerImpl::createDirectoryHandler( CreateDirectoryHandler createDirectoryHandler )
{
  createDirectoryHandlerV = std::move( createDirectoryHandler );
  return *this;
}

MediaSetCompiler& MediaSetCompilerImpl::checkFileExistenceHandler( CheckFileExistenceHandler checkFileExistenceHandler )
{
  checkFileExistenceHandlerV = std::move( checkFileExistenceHandler );
  return *this;
}

MediaSetCompiler &MediaSetCompilerImpl::createFileHandler( CreateFileHandler createFileHandler )
{
  createFileHandlerV = std::move( createFileHandler );
  return *this;
}

MediaSetCompiler &MediaSetCompilerImpl::writeFileHandler( WriteFileHandler writeFileHandler )
{
  writeFileHandlerV = std::move( writeFileHandler );
  return *this;
}

MediaSetCompiler &MediaSetCompilerImpl::readFileHandler( ReadFileHandler readFileHandler )
{
  readFileHandlerV = std::move( readFileHandler );
  return *this;
}

MediaSetCompiler &MediaSetCompilerImpl::arinc665Version( const SupportedArinc665Version version )
{
  arinc665VersionV = version;
  return *this;
}

MediaSetCompiler &MediaSetCompilerImpl::createBatchFiles( const FileCreationPolicy createBatchFiles )
{
  createBatchFilesV = createBatchFiles;
  return *this;
}

MediaSetCompiler &MediaSetCompilerImpl::createLoadHeaderFiles( const FileCreationPolicy createLoadHeaderFiles )
{
  createLoadHeaderFilesV = createLoadHeaderFiles;
  return *this;
}

void MediaSetCompilerImpl::operator()()
{
  if ( !mediaSetV || !createMediumHandlerV || !createDirectoryHandlerV
    || !checkFileExistenceHandlerV || !createFileHandlerV || !writeFileHandlerV
    || !readFileHandlerV )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "Invalid state of exporter" } );
  }

  SPDLOG_INFO( "Export Media Set '{}'", mediaSetV->partNumber() );

  // first export medium (directories and regular files)
  for ( MediumNumber mediumNumber{ 1U }; mediumNumber <= mediaSetV->lastMediumNumber(); ++mediumNumber )
  {
    // create the medium (i.e. create directory)
    createMediumHandlerV( mediumNumber );

    // export regular files
    for ( const auto &file : mediaSetV->regularFiles() )
    {
      exportRegularFile( file );
    }

    // export subdirectories
    for ( const auto &directory : mediaSetV->subdirectories( mediumNumber ) )
    {
      exportDirectory( mediumNumber, directory );
    }
  }

  // export load headers
  for ( const auto &load : mediaSetV->recursiveLoads() )
  {
    exportLoad( load );
  }

  // export batch files
  for ( const auto &batch : mediaSetV->recursiveBatches() )
  {
    exportBatch( batch );
  }

  // export list of loads for all media
  exportListOfLoads();

  // export "list of batches" file (if present) for all media
  if ( mediaSetV->numberOfBatches() != 0U )
  {
    exportListOfBatches();
  }

  // export "list of files" for all media
  exportListOfFiles();
}

void MediaSetCompilerImpl::exportDirectory(
  const MediumNumber &mediumNumber,
  const Media::ConstDirectoryPtr &directory )
{
  SPDLOG_INFO( "Export Directory to [{}]:'{}'", mediumNumber.toString(), directory->path().string() );

  createDirectoryHandlerV( mediumNumber, directory );

  // export regular files
  for ( const auto &file : directory->regularFiles() )
  {
    exportRegularFile( file );
  }

  // export sub-directories
  for ( const auto &subDirectory : directory->subdirectories() )
  {
    exportDirectory( mediumNumber, subDirectory );
  }
}

void MediaSetCompilerImpl::exportRegularFile( const Media::ConstRegularFilePtr &file )
{
  SPDLOG_INFO( "Export Regular File to [{}]:'{}'", file->effectiveMediumNumber().toString(), file->path().string() );

  // regular file mus be created by callback
  createFileHandlerV( file );
}

void MediaSetCompilerImpl::exportLoad( const Media::ConstLoadPtr &load )
{
  SPDLOG_INFO( "Export Load to [{}]:'{}'", load->effectiveMediumNumber().toString(), load->path().string() );

  // check load header creation policy
  switch ( createLoadHeaderFilesV )
  {
    case FileCreationPolicy::None:
      createFileHandlerV( load );
      break;

    case FileCreationPolicy::NoneExisting:
      if ( checkFileExistenceHandlerV( load ) )
      {
        createFileHandlerV( load );
      }
      else
      {
        createLoadHeaderFile( *load );
      }
      break;

    case FileCreationPolicy::All:
      createLoadHeaderFile( *load );
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Invalid value of createLoadHeaderFiles" } );
  }
}

void MediaSetCompilerImpl::exportBatch( const Media::ConstBatchPtr &batch )
{
  SPDLOG_INFO( "Export Batch to [{}]:'{}'", batch->effectiveMediumNumber().toString(), batch->path().string() );

  // check batch file creation policy
  switch ( createBatchFilesV )
  {
    case FileCreationPolicy::None:
      createFileHandlerV( batch );
      break;

    case FileCreationPolicy::NoneExisting:
      // check batch file creation policy
      if ( checkFileExistenceHandlerV( batch ) )
      {
        createFileHandlerV( batch );
      }
      else
      {
        createBatchFile( *batch );
      }
      break;

    case FileCreationPolicy::All:
      createBatchFile( *batch );
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Invalid value of createBatchFiles" } );
  }
}

void MediaSetCompilerImpl::exportListOfLoads() const
{
  Files::LoadListFile loadListFile{ arinc665VersionV };

  loadListFile.mediaSetPn( std::string{ mediaSetV->partNumber() } );
  loadListFile.numberOfMediaSetMembers( mediaSetV->lastMediumNumber() );

  /* add all loads to "list of loads" file */
  for ( const auto &load : mediaSetV->recursiveLoads() )
  {
    auto thwIds{ load->targetHardwareIds() };
    loadListFile.load( Files::LoadInfo{
      std::string{ load->partNumber() },
      std::string{ load->name() },
      load->effectiveMediumNumber(),
      Files::LoadInfo::ThwIds{ thwIds.begin(), thwIds.end() } } );
  }

  auto userDefinedData{ mediaSetV->loadsUserDefinedData() };
  loadListFile.userDefinedData( Helper::RawData{ userDefinedData.begin(), userDefinedData.end() } );

  for ( MediumNumber mediumNumber{ 1U }; mediumNumber <= mediaSetV->lastMediumNumber(); ++mediumNumber )
  {
    const std::filesystem::path filename{ "/" / std::filesystem::path{ ListOfLoadsName } };

    SPDLOG_INFO( "Export List of Loads to [{}]:'{}'", mediumNumber.toString(), filename.string() );

    loadListFile.mediaSequenceNumber( mediumNumber );

    writeFileHandlerV( mediumNumber, filename, static_cast< Helper::RawData >( loadListFile ) );
  }
}

void MediaSetCompilerImpl::exportListOfBatches() const
{
  Files::BatchListFile batchListFile{ arinc665VersionV };
  batchListFile.mediaSetPn( std::string{ mediaSetV->partNumber() } );
  batchListFile.numberOfMediaSetMembers( mediaSetV->lastMediumNumber() );

  /* add all batches to batches list */
  for ( const auto &batch : mediaSetV->recursiveBatches() )
  {
    batchListFile.batch( Files::BatchInfo{
      std::string{ batch->partNumber() },
      std::string{ batch->name() },
      batch->effectiveMediumNumber() } );
  }

  auto userDefinedData{ mediaSetV->batchesUserDefinedData() };
  batchListFile.userDefinedData( Helper::RawData{ userDefinedData.begin(), userDefinedData.end() } );

  for ( MediumNumber mediumNumber{ 1U }; mediumNumber <= mediaSetV->lastMediumNumber(); ++mediumNumber )
  {
    const std::filesystem::path filename{ "/" / std::filesystem::path{ ListOfBatchesName } };

    SPDLOG_INFO( "Export List of Batches to [{}]:'{}'", mediumNumber.toString(), filename.string() );

    batchListFile.mediaSequenceNumber( mediumNumber );

    writeFileHandlerV( mediumNumber, filename, static_cast< Helper::RawData >( batchListFile ) );
  }
}

void MediaSetCompilerImpl::exportListOfFiles() const
{
  Files::FilesInfo filesInfo{};

  /* add all files, load header files, and batch files to file list */
  for ( const auto &file : mediaSetV->recursiveFiles() )
  {
    const auto &[ fileCrc, fileCheckValue ]{
      fileCrcCheckValue( file->effectiveMediumNumber(), file->path(), file->effectiveCheckValueType() ) };

    filesInfo.emplace_back( Files::FileInfo{
      .filename = std::string{ file->name() },
      .pathName = Files::Arinc665File::encodePath( file->path().parent_path() ),
      .memberSequenceNumber = file->effectiveMediumNumber(),
      .crc = fileCrc,
      .checkValue = fileCheckValue } );
  }

  Arinc665::Files::FileListFile fileListFile{ arinc665VersionV };
  fileListFile.mediaSetPn( std::string{ mediaSetV->partNumber() } );
  fileListFile.numberOfMediaSetMembers( mediaSetV->lastMediumNumber() );
  auto userDefinedData{ mediaSetV->filesUserDefinedData() };
  fileListFile.userDefinedData( Helper::RawData{ userDefinedData.begin(), userDefinedData.end() } );
  fileListFile.checkValueType( mediaSetV->effectiveListOfFilesCheckValueType() );

  for ( MediumNumber mediumNumber{ 1U }; mediumNumber <= mediaSetV->lastMediumNumber(); ++mediumNumber )
  {
    const std::filesystem::path filename{ "/" / std::filesystem::path{ ListOfFilesName } };

    SPDLOG_INFO( "Export List of Files to [{}]:'{}'", mediumNumber.toString(), filename.string() );

    fileListFile.mediaSequenceNumber( mediumNumber );

    // clear file list
    fileListFile.files().clear();

    // add list of loads
    const auto&[ listOfLoadsCrc, listOfLoadsCheckValue ]{ fileCrcCheckValue(
      mediumNumber,
      "/" + std::string{ ListOfLoadsName },
      mediaSetV->effectiveListOfLoadsCheckValueType() ) };

    fileListFile.file( {
      std::string{ ListOfLoadsName },
      Files::Arinc665File::encodePath( "/" ),
      mediumNumber,
      listOfLoadsCrc,
      listOfLoadsCheckValue } );

    // add list of batches - if present
    if ( mediaSetV->numberOfBatches() != 0 )
    {
      const auto &[ listOfBatchesFileCrc, listOfBatchesFileCheckValue ]{ fileCrcCheckValue(
        mediumNumber,
        "/" + std::string{ ListOfBatchesName },
        mediaSetV->effectiveListOfBatchesCheckValueType() ) };

      fileListFile.file( {
        std::string{ ListOfBatchesName },
        Files::Arinc665File::encodePath( "/" ),
        mediumNumber,
        listOfBatchesFileCrc,
        listOfBatchesFileCheckValue } );
    }

    // add Files
    fileListFile.files().insert( fileListFile.files().end(), filesInfo.begin(), filesInfo.end() );

    writeFileHandlerV( mediumNumber, filename, static_cast< Helper::RawData >( fileListFile ) );
  }
}

void MediaSetCompilerImpl::createLoadHeaderFile( const Media::Load &load ) const
{
  Files::LoadHeaderFile loadHeaderFile{ arinc665VersionV };
  loadHeaderFile.partFlags( load.partFlags() );
  loadHeaderFile.partNumber( std::string{ load.partNumber() } );
  for ( const auto &[ thwId, positions ] : load.targetHardwareIdPositions() )
  {
    loadHeaderFile.targetHardwareId( thwId );
    loadHeaderFile.targetHardwareIdPositions(
      thwId,
      Files::LoadHeaderFile::Positions{ positions.begin(), positions.end() } );
  }
  loadHeaderFile.loadType( load.loadType() );

  // Process data files and add info to load header.
  for ( const auto &file : load.dataFiles( true ) )
  {
    loadHeaderFile.dataFile( loadFileInformation( file ) );
  }

  // Process support files and add info to load header.
  for ( const auto &file: load.supportFiles( true ) )
  {
    loadHeaderFile.supportFile( loadFileInformation( file ) );
  }

  // User Defined Data
  auto userDefinedData{ load.userDefinedData() };
  loadHeaderFile.userDefinedData( Helper::RawData{ userDefinedData.begin(), userDefinedData.end() } );

  // Set Check Value Type before Raw File generation (for Space Reserving)
  loadHeaderFile.loadCheckValueType( load.effectiveLoadCheckValueType() );

  // RAW load header used for load Check Value and CRC calculation
  auto rawLoadHeader{ Helper::RawData( loadHeaderFile ) };

  // Calculate Load Check Value (Only for supported version)
  if ( SupportedArinc665Version::Supplement345 == arinc665VersionV )
  {
    auto checkValueGenerator{ Arinc649::CheckValueGenerator::create( load.effectiveLoadCheckValueType() ) };
    assert( checkValueGenerator );

    Files::LoadHeaderFile::processLoadCheckValue( rawLoadHeader, *checkValueGenerator );

    // load data files for Load Check Value.
    for ( const auto &[ file, partNumber, checkValueType ] : load.dataFiles() )
    {
      auto rawDataFile{ readFileHandlerV( file->effectiveMediumNumber(), file->path() ) };

      checkValueGenerator->process( std::as_bytes( Helper::RawDataSpan{ rawDataFile } ) );
    }

    // load support files for Load Check Value.
    for ( const auto &[ file, partNumber, checkValueType ] : load.supportFiles() )
    {
      auto rawSupportFile{ readFileHandlerV( file->effectiveMediumNumber(), file->path() ) };

      checkValueGenerator->process( std::as_bytes( Helper::RawDataSpan{ rawSupportFile } ) );
    }

    Files::LoadHeaderFile::encodeLoadCheckValue( rawLoadHeader, checkValueGenerator->checkValue() );
  }


  // Calculate load CRC
  Arinc649::Arinc649Crc32 loadCrc{};

  Files::LoadHeaderFile::processLoadCrc( rawLoadHeader, loadCrc );

  // load data files for load CRC.
  for ( const auto &[ file, partNumber, checkValueType ] : load.dataFiles() )
  {
    auto rawDataFile{ readFileHandlerV( file->effectiveMediumNumber(), file->path() ) };

    loadCrc.process_bytes( std::to_address( rawDataFile.begin() ), rawDataFile.size() );
  }

  // load support files for load CRC.
  for ( const auto &[ file, partNumber, checkValueType ] : load.supportFiles() )
  {
    auto rawSupportFile{ readFileHandlerV( file->effectiveMediumNumber(), file->path() ) };

    loadCrc.process_bytes( std::to_address( rawSupportFile.begin() ), rawSupportFile.size() );
  }

  // set load CRC
  Files::LoadHeaderFile::encodeLoadCrc( rawLoadHeader, loadCrc.checksum() );

  // Write Load Header File
  writeFileHandlerV( load.effectiveMediumNumber(), load.path(), rawLoadHeader );
}

Files::LoadFileInfo MediaSetCompilerImpl::loadFileInformation( const Media::ConstLoadFile &loadFile ) const
{
  const auto &[ file, partNumber, checkValueType ] = loadFile;

  // read file
  const auto rawDataFile{ readFileHandlerV( file->effectiveMediumNumber(), file->path() ) };

  return Files::LoadFileInfo{
    .filename = std::string{ file->name() },
    .partNumber = partNumber,
    .length = rawDataFile.size(),
    .crc = Files::Arinc665File::calculateChecksum( rawDataFile ),
    .checkValue = Arinc649::CheckValueGenerator::checkValue(
      checkValueType.value_or( Arinc649::CheckValueType::NotUsed ),
      std::as_bytes( Helper::ConstRawDataSpan{ rawDataFile } ) ).value_or( Arinc649::CheckValue::NoCheckValue) };
}

void MediaSetCompilerImpl::createBatchFile( const Media::Batch &batch ) const
{
  Files::BatchFile batchFile{ arinc665VersionV };
  batchFile.partNumber( std::string{ batch.partNumber() } );
  batchFile.comment( std::string{ batch.comment() } );

  // iterate over targets
  for ( const auto &[ targetHwId, loads ] : batch.targets() )
  {
    Files::BatchLoadsInfo batchLoadsInfo{};

    // iterate over loads
    for ( const auto &load : loads )
    {
      // add load to target batch information
      batchLoadsInfo.emplace_back( Files::BatchLoadInfo{
        .headerFilename = std::string{ load->name() },
        .partNumber = std::string{ load->partNumber() } } );
    }

    // add target
    batchFile.targetHardware( Files::BatchTargetInfo{ targetHwId, std::move( batchLoadsInfo ) } );
  }

  writeFileHandlerV( batch.effectiveMediumNumber(), batch.path(), static_cast< Helper::RawData >( batchFile ) );
}

std::tuple< uint16_t, Arinc649::CheckValue > MediaSetCompilerImpl::fileCrcCheckValue(
  const MediumNumber mediumNumber,
  const std::filesystem::path &filename,
  const Arinc649::CheckValueType checkValueType ) const
{
  auto checkValueGenerator{ Arinc649::CheckValueGenerator::create( checkValueType ) };
  assert( checkValueGenerator );

  const auto rawFile{ readFileHandlerV( mediumNumber, filename ) };

  const auto crc{ Files::Arinc665File::calculateChecksum( rawFile ) };

  checkValueGenerator->process( std::as_bytes( Helper::ConstRawDataSpan{ rawFile } ) );
  const auto checkValue = checkValueGenerator->checkValue();

  return { crc, checkValue };
}

}
