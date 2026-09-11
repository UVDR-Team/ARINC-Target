// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::MediaSetDecompilerImpl.
 **/

#include "MediaSetDecompilerImpl.hpp"

#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/Batch.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/CheckValueGenerator.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <ranges>

namespace Arinc665::Utils {

MediaSetDecompiler &MediaSetDecompilerImpl::fileSizeHandler( FileSizeHandler fileSizeHandler )
{
  fileSizeHandlerV = std::move( fileSizeHandler );
  return *this;
}

MediaSetDecompiler &MediaSetDecompilerImpl::readFileHandler( ReadFileHandler readFileHandler )
{
  readFileHandlerV = std::move( readFileHandler );
  return *this;
}

MediaSetDecompiler &MediaSetDecompilerImpl::progressHandler( ProgressHandler progressHandler )
{
  progressHandlerV = std::move( progressHandler );
  return *this;
}

MediaSetDecompiler &MediaSetDecompilerImpl::checkFileIntegrity( const bool checkFileIntegrity ) noexcept
{
  checkFileIntegrityV = checkFileIntegrity;
  return *this;
}

MediaSetDecompilerResult MediaSetDecompilerImpl::operator()()
{
  if ( !fileSizeHandlerV || !readFileHandlerV )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{}
      << Helper::AdditionalInfo{ "Missing file size or read file handler" } );
  }

  // create Media set
  mediaSetV = Media::MediaSet::create();

  // 1st Medium
  loadFirstMedium();

  // load further media
  loadFurtherMedia();

  // finally, add all files (regular, load headers, batches) to the media set
  files();

  return { std::move( mediaSetV ), std::move( checkValuesV ) };
}

void MediaSetDecompilerImpl::loadFirstMedium()
{
  // Load "list of files" file
  fileListFileV = readFileHandlerV( MediumNumber{ 1U }, Arinc665::ListOfFilesName );

  if ( fileListFileV.mediaSequenceNumber() != MediumNumber{ 1U } )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "File List File of 1st medium incorrect" } );
  }

  // Call progress handler
  if ( progressHandlerV )
  {
    progressHandlerV(
      fileListFileV.mediaSetPn(),
      { fileListFileV.mediaSequenceNumber(), fileListFileV.numberOfMediaSetMembers() } );
  }

  // indicator, that LOADS.LUM present in FILES.LUM
  bool listOfLoadsFilePresent{ false };

  for ( const auto &fileInfo : fileListFileV.files() )
  {
    // get file type
    // skip list files and handle load headers and batch files separate
    if ( const auto fileType{ Files::Arinc665File::fileType( fileInfo.filename ) }; fileType )
    {
      switch ( *fileType )
      {
        using enum FileType;

        case FileList:
          BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "File List File not expected to be in FILES.LUM" } );

        case LoadList:
          // check that List of Loads is in Root Directory
          if ( fileInfo.pathName != "\\" )
          {
            BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "LOADS.LUM not in Root Directory" } );
          }

          listOfLoadsFilePresent = true;

          // store load list file check value
          mediaSetV->listOfLoadsCheckValueType( fileInfo.checkValue.type() );

          // List of Loads not added to file information
          continue ;

        case BatchList:
          // check that List of Loads is in Root Directory
          if ( fileInfo.pathName != "\\" )
          {
            BOOST_THROW_EXCEPTION( Arinc665Exception()
              << Helper::AdditionalInfo{ "BATCHES.LUM not in Root Directory" } );
          }

          // set batch list file present indicator
          batchListFilePresentV = true;

          // store batch list file check value
          mediaSetV->listOfBatchesCheckValueType( fileInfo.checkValue.type() );

          // List of Batches not added to file information
          continue ;

        default:
          break;
      }
    }

    // update files information
    filesInfosV.emplace( fileInfo.filename, fileInfo );
  }

  // Load List File

  // Check Existence of Load List File within Files Information
  if ( !listOfLoadsFilePresent )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "Load List not in FILES.LUM" } );
  }

  // check file integrity on current medium
  checkMediumFiles( MediumNumber{ 1U } );

  // store list of files user defined data
  auto filesUserDefinedData{ fileListFileV.userDefinedData() };
  mediaSetV->filesUserDefinedData( Helper::RawData{ filesUserDefinedData.begin(), filesUserDefinedData.end() } );
  // store file list file check value
  mediaSetV->listOfFilesCheckValueType( fileListFileV.checkValueType() );
  // Set Media Set Parameter
  mediaSetV->partNumber( std::string{ fileListFileV.mediaSetPn() } );

  // Load "list of loads" file
  loadListFileV = readFileHandlerV( MediumNumber{ 1U }, Arinc665::ListOfLoadsName );

  for ( const auto &load : loadListFileV.loads() )
  {
    // check existence of load header file
    const auto fileIt{ filesInfosV.find( load.headerFilename ) };

    if ( fileIt == filesInfosV.end() )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "load header file not found" }
        << boost::errinfo_file_name{ load.headerFilename } );
    }

    // checks that the load list and file list entry map to the same file entry
    if ( load != fileIt->second )
    {
      BOOST_THROW_EXCEPTION(
        Arinc665Exception()
        << Helper::AdditionalInfo{ "data inconsistency" }
        << boost::errinfo_file_name{ load.headerFilename } );
    }

    // add load to "loads information"
    loadsInfosV.try_emplace( load.headerFilename, load );
  }

  // store list of loads user defined data
  auto loadsUserDefinedData{ loadListFileV.userDefinedData() };
  mediaSetV->loadsUserDefinedData( Helper::RawData{ loadsUserDefinedData.begin(), loadsUserDefinedData.end() } );

  // Load "list of batches" file
  if ( batchListFilePresentV )
  {
    batchListFileV = readFileHandlerV( MediumNumber{ 1U }, Arinc665::ListOfBatchesName );

    for ( const auto &batch : batchListFileV.batches() )
    {
      // check existence of load header file
      const auto fileIt{ filesInfosV.find( batch.filename ) };

      if ( fileIt == filesInfosV.end() )
      {
        BOOST_THROW_EXCEPTION( Arinc665Exception{}
          << Helper::AdditionalInfo{ "batch file not found" }
          << boost::errinfo_file_name{ batch.filename } );
      }

      // checks that the load list and file list entry maps to the same file entry
      if ( batch != fileIt->second )
      {
        BOOST_THROW_EXCEPTION(
          Arinc665Exception{}
          << Helper::AdditionalInfo{ "data inconsistency" }
          << boost::errinfo_file_name{ batch.filename } );
      }

      // add batch to "batches information"
      batchesInfosV.try_emplace( batch.filename, batch );
    }

    // store list of batches user defined data
    auto batchesUserDefinedData{ batchListFileV.userDefinedData() };
    mediaSetV->batchesUserDefinedData(
      Helper::RawData{ batchesUserDefinedData.begin(), batchesUserDefinedData.end() } );
  }
}

void MediaSetDecompilerImpl::loadFurtherMedia() const
{
  // iterate over media
  for ( MediumNumber mediumNumber{ 2U }; mediumNumber <= fileListFileV.numberOfMediaSetMembers(); ++mediumNumber )
  {
    // Call progress handler
    if ( progressHandlerV )
    {
      progressHandlerV( fileListFileV.mediaSetPn(), { mediumNumber, fileListFileV.numberOfMediaSetMembers() } );
    }

    // Load "list of files" file

    // compare current list of files to first one
    if (
      const Files::FileListFile mediumFileListFile{ readFileHandlerV( mediumNumber, Arinc665::ListOfFilesName ) };
      !mediumFileListFile.belongsToSameMediaSet( fileListFileV )
        || ( mediumNumber != mediumFileListFile.mediaSequenceNumber() ) )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "inconsistent file list file" }
        << boost::errinfo_file_name{ std::string{ Arinc665::ListOfFilesName } } );
    }

    // check file integrity on current medium
    checkMediumFiles( mediumNumber );

    // Load "List of Loads" file

    // check against stored version
    if (
      const Files::LoadListFile mediumLoadListFile{ readFileHandlerV( mediumNumber, Arinc665::ListOfLoadsName ) };
      !mediumLoadListFile.belongsToSameMediaSet( loadListFileV )
        || ( mediumNumber != mediumLoadListFile.mediaSequenceNumber() ) )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ std::string{ Arinc665::ListOfLoadsName }
          + " is not consistent to other loads list" } );
    }

    // Load "List of Batches" file
    if ( batchListFilePresentV )
    {
      // check against stored version
      if (
        const Files::BatchListFile mediumBatchListFile{ readFileHandlerV( mediumNumber, Arinc665::ListOfBatchesName ) };
        !mediumBatchListFile.belongsToSameMediaSet( batchListFileV )
          || ( mediumNumber != mediumBatchListFile.mediaSequenceNumber() ) )
      {
        BOOST_THROW_EXCEPTION(
          Arinc665Exception() << Helper::AdditionalInfo{
            std::string{ Arinc665::ListOfBatchesName }
            + " is not consistent to other batches list" } );
      }
    }
  }
}

void MediaSetDecompilerImpl::files()
{
  // iterate over all files
  for ( const auto &[ fileName, fileInfo ] : filesInfosV )
  {
    // get directory, where file will be placed into.
    const auto parent{ checkCreateDirectory( fileInfo.path().parent_path() ) };

    // is file a load header file?
    if ( const auto loadInfo{ loadsInfosV.find( fileName ) }; loadInfo != loadsInfosV.end() )
    {
      // check that load is not present in batches
      if ( const auto batchInfo{ batchesInfosV.find( fileName ) }; batchInfo != batchesInfosV.end() )
      {
        BOOST_THROW_EXCEPTION( Arinc665Exception{}
          << Helper::AdditionalInfo{ "Load file also in batch list present" }
          << boost::errinfo_file_name{ fileName } );
      }

      loadFile( *parent, fileInfo, loadInfo->second );
    }
    // is file a batch file
    else if ( const auto batchInfo{ batchesInfosV.find( fileName ) }; batchInfo != batchesInfosV.end() )
    {
      batchFile( *parent, fileInfo, batchInfo->second );
    }
    else
    {
      regularFile( *parent, fileInfo );
    }
  }

  // finally, fill loads and batches with data

  // iterate over load headers
  for ( const auto &[ file, loadInfo ] : loadsV )
  {
    addLoad( *file, loadInfo.first, loadInfo.second );
  }

  // iterate over batches
  for ( const auto &[ file, batchInfo ] : batchesV )
  {
    addBatch( *file, batchInfo.first, batchInfo.second );
  }
}

void MediaSetDecompilerImpl::regularFile( Media::ContainerEntity &parent, const Files::FileInfo &fileInfo )
{
  SPDLOG_TRACE( "Regular File '{}'", fileInfo.path().generic_string() );

  // create file
  auto file{ parent.addRegularFile( fileInfo.filename, fileInfo.memberSequenceNumber ) };
  assert( file );

  // set check value indicator
  file->checkValueType( fileInfo.checkValue.type() );

  // update check values (CRC and Check Value if provided)
  checkValuesV[ file ].emplace( Arinc649::CheckValue::crc16( fileInfo.crc ) );

  if ( Arinc649::CheckValue::NoCheckValue != fileInfo.checkValue )
  {
    checkValuesV[ file ].emplace( fileInfo.checkValue );
  }

  // add to deferred load handling
  regularFilesV.try_emplace( std::move( file ), fileInfo );
}

void MediaSetDecompilerImpl::loadFile(
  Media::ContainerEntity &parent,
  const Files::FileInfo &fileInfo,
  const Files::LoadInfo &loadInfo )
{
  SPDLOG_TRACE( "Load Header File '{}'", fileInfo.path().generic_string() );

  // create the load
  auto load{ parent.addLoad( loadInfo.headerFilename, loadInfo.memberSequenceNumber ) };
  assert( load );

  // set check value indicator
  load->checkValueType( fileInfo.checkValue.type() );

  // update check values (CRC and Check Value if provided)
  checkValuesV[ load ].emplace( Arinc649::CheckValue::crc16( fileInfo.crc ) );

  if ( Arinc649::CheckValue::NoCheckValue != fileInfo.checkValue )
  {
    checkValuesV[ load ].emplace( fileInfo.checkValue );
  }

  // add to deferred load handling
  loadsV.try_emplace( std::move( load ), fileInfo, loadInfo );
}

void MediaSetDecompilerImpl::batchFile(
  Media::ContainerEntity &parent,
  const Files::FileInfo &fileInfo,
  const Files::BatchInfo &batchInfo )
{
  SPDLOG_TRACE( "Batch File '{}'", fileInfo.path().generic_string() );

  // create batch
  auto batch{ parent.addBatch( batchInfo.filename, batchInfo.memberSequenceNumber ) };
  assert( batch );

  // set check value indicator
  batch->checkValueType( fileInfo.checkValue.type() );

  // update check values (CRC and Check Value if provided)
  checkValuesV[ batch ].emplace( Arinc649::CheckValue::crc16( fileInfo.crc ) );

  if ( Arinc649::CheckValue::NoCheckValue != fileInfo.checkValue )
  {
    checkValuesV[ batch ].emplace( fileInfo.checkValue );
  }

  // add to deferred batch handling
  batchesV.try_emplace( std::move( batch ), fileInfo, batchInfo );
}

void MediaSetDecompilerImpl::addLoad(
  Media::Load &load,
  const Files::FileInfo &fileInfo,
  const Files::LoadInfo &loadInfo )
{
  // decode load header
  const auto rawLoadHeaderFile{ readFileHandlerV( fileInfo.memberSequenceNumber, fileInfo.path() ) };
  const Files::LoadHeaderFile loadHeaderFile{ rawLoadHeaderFile };

  // validate load part number to load information
  if ( loadInfo.partNumber != loadHeaderFile.partNumber() )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665Exception()
      << Helper::AdditionalInfo{ "Load part number inconsistent" }
      << boost::errinfo_file_name{ std::string{ loadInfo.headerFilename } } );
  }

  // validate THW IDs of load header against the list of loads
  if (
    std::multiset< std::string, std::less<> >{ loadInfo.targetHardwareIds.begin(), loadInfo.targetHardwareIds.end() }
    != std::multiset< std::string, std::less<> >{
      loadHeaderFile.targetHardwareIds().begin(),
      loadHeaderFile.targetHardwareIds().end() } )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665Exception()
      << Helper::AdditionalInfo{ "Load THW IDs inconsistent" }
      << boost::errinfo_file_name{ std::string{ loadInfo.headerFilename } } );
  }

  load.partFlags( loadHeaderFile.partFlags() );
  load.partNumber( std::string{ loadHeaderFile.partNumber() } );
  load.loadType( loadHeaderFile.loadType() );
  Media::Load::TargetHardwareIdPositions thwIdsPositions{};
  for ( const auto &[ thwId, positions ] : loadHeaderFile.targetHardwareIdsPositions() )
  {
    thwIdsPositions.try_emplace( thwId, positions.begin(), positions.end() );
  }
  for ( const auto &thwId : loadHeaderFile.targetHardwareIds() )
  {
    // if not previously added - add it now with empty positions
    thwIdsPositions.try_emplace( thwId );
  }
  load.targetHardwareIdPositions( std::move( thwIdsPositions ) );

  // Load Check CRC and Load Check Value
  Arinc649::Arinc649Crc32 loadCrc{};
  const auto loadCheckValueGenerator{ Arinc649::CheckValueGenerator::create( loadHeaderFile.loadCheckValueType() ) };
  assert( loadCheckValueGenerator );

  if ( checkFileIntegrityV )
  {
    Files::LoadHeaderFile::processLoadCrc( rawLoadHeaderFile, loadCrc );
    Files::LoadHeaderFile::processLoadCheckValue( rawLoadHeaderFile, *loadCheckValueGenerator );
  }

  // iterate over data files
  for ( const auto &loadFileInfo : loadHeaderFile.dataFiles() )
  {
    // start search in parent directory of load (according ARINC 665-5)
    const auto dataFilePtr{ loadFile( *load.parent(), loadFileInfo.filename, loadFileInfo.crc ) };

    const auto dataFileInfo{ regularFilesV.find( dataFilePtr ) };
    assert( dataFileInfo != regularFilesV.end() );

    // perform file check
    // in ARINC 665-2 File Size of Data File is stored as multiple of 16 bit
    checkLoadFile(
      loadCrc,
      *loadCheckValueGenerator,
      dataFileInfo->second,
      loadFileInfo,
      loadHeaderFile.arincVersion() == SupportedArinc665Version::Supplement2 );

    load.dataFile( dataFilePtr, loadFileInfo.partNumber, loadFileInfo.checkValue.type() );

    // Add check value if provided - CRC 16 is not added, as it is handled
    // within addFiles
    if ( Arinc649::CheckValue::NoCheckValue != loadFileInfo.checkValue )
    {
      checkValuesV[ dataFilePtr ].emplace( loadFileInfo.checkValue );
    }
  }

  // iterate over support files
  for ( const auto &loadFileInfo : loadHeaderFile.supportFiles() )
  {
    // start search in parent directory of load (according ARINC 665-5)
    auto supportFilePtr{ loadFile( *load.parent(), loadFileInfo.filename, loadFileInfo.crc ) };

    const auto supportFileInfo{ regularFilesV.find( supportFilePtr ) };
    assert( supportFileInfo != regularFilesV.end() );

    checkLoadFile( loadCrc, *loadCheckValueGenerator, supportFileInfo->second, loadFileInfo, false );

    load.supportFile( supportFilePtr, loadFileInfo.partNumber, loadFileInfo.checkValue.type() );

    // Add check value if provided - CRC 16 is not added, as it is handled
    // within addFiles
    if ( Arinc649::CheckValue::NoCheckValue != loadFileInfo.checkValue )
    {
      checkValuesV[ supportFilePtr ].emplace( loadFileInfo.checkValue );
    }
  }

  // Check Load CRC and Load Check Value
  if ( checkFileIntegrityV )
  {
    if ( Files::LoadHeaderFile::decodeLoadCrc( rawLoadHeaderFile ) != loadCrc.checksum() )
    {
      BOOST_THROW_EXCEPTION(
        Arinc665Exception()
        << Helper::AdditionalInfo{ "Load CRC inconsistent" }
        << boost::errinfo_file_name{ fileInfo.filename } );
    }

    if ( Files::LoadHeaderFile::decodeLoadCheckValue( rawLoadHeaderFile ) != loadCheckValueGenerator->checkValue() )
    {
      BOOST_THROW_EXCEPTION(
        Arinc665Exception()
        << Helper::AdditionalInfo{ "Load Check Value inconsistent" }
        << boost::errinfo_file_name{ fileInfo.filename } );
    }
  }

  // User Defined Data
  auto loadUserDefinedData{ loadHeaderFile.userDefinedData() };
  load.userDefinedData( Helper::RawData{ loadUserDefinedData.begin(), loadUserDefinedData.end() } );
  // Load Check Value
  load.loadCheckValueType( loadHeaderFile.loadCheckValueType() );
}

Media::RegularFilePtr MediaSetDecompilerImpl::loadFile(
  Media::ContainerEntity &parent,
  std::string_view filename,
  const uint16_t crc ) const
{
  auto files{ mediaSetV->recursiveRegularFiles( filename ) };

  // no file found is a failure
  if ( files.empty() )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665Exception()
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ std::string{ filename } } );
  }

  // only one file present on disk
  if ( 1U == files.size() )
  {
    return files.front();
  }

  // search for files only in parent container

  files = parent.recursiveRegularFiles( filename );

  // no file found is a failure
  if ( files.empty() )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665Exception()
      << Helper::AdditionalInfo{ "File not found" }
      << boost::errinfo_file_name{ std::string{ filename } } );
  }

  // only one file present in container
  if ( 1U == files.size() )
  {
    return files.front();
  }

  // find file with same CRC
  for ( const auto &file : files )
  {
    for ( const auto &checkValue : checkValuesV.at( file ) )
    {
      if ( Arinc649::CheckValueType::Crc16 == checkValue.type() )
      {
        if (
          const auto crcConverted{ Arinc649::CheckValue::crc16( checkValue ) };
          crcConverted && ( crc == *crcConverted ) )
        {
          // CRC matches
          return file;
        }
      }
    }
  }

  // choose first found file
  return files.front();
}

void MediaSetDecompilerImpl::addBatch(
  Media::Batch &batch,
  const Files::FileInfo &fileInfo,
  const Files::BatchInfo &batchInfo )
{
  // Decode batch File
  Files::BatchFile batchFile{ readFileHandlerV( fileInfo.memberSequenceNumber, fileInfo.path() ) };

  // validate batch part number to batch information
  if ( batchInfo.partNumber != batchFile.partNumber() )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665Exception()
      << Helper::AdditionalInfo{ "Batch part number inconsistent" }
      << boost::errinfo_file_name{ std::string{ batchInfo.filename } } );
  }

  batch.partNumber( std::string{ batchFile.partNumber() } );
  batch.comment( std::string{ batchFile.comment() } );

  // iterate over target hardware
  for ( const auto &targetHardware : batchFile.targetsHardware() )
  {
    Media::ConstLoads batchLoads{};

    // iterate over loads
    for ( const auto& load : targetHardware.loads )
    {
      // start search in parent directory of batch (according ARINC 665-5)
      const auto loads{ mediaSetV->recursiveLoads( load.headerFilename ) };

      if ( loads.empty() )
      {
        BOOST_THROW_EXCEPTION(
          Arinc665Exception()
          << Helper::AdditionalInfo{ "Load not found" }
          << boost::errinfo_file_name{ std::string{ load.headerFilename } } );
      }

      if ( loads.size() > 1U )
      {
        BOOST_THROW_EXCEPTION(
          Arinc665Exception()
          << Helper::AdditionalInfo{ "Load not unique" }
          << boost::errinfo_file_name{ std::string{ load.headerFilename } } );
      }

      // check that Part Number Information matches
      if ( loads.front()->partNumber() != load.partNumber )
      {
        BOOST_THROW_EXCEPTION(
          Arinc665Exception()
          << Helper::AdditionalInfo{ "Load part Number does not match Batch Info" }
          << boost::errinfo_file_name{ std::string{ load.headerFilename } } );
      }

      batchLoads.emplace_back( loads.front() );
    }

    // add Target Hardware/ Position
    batch.target( targetHardware.targetHardwareIdPosition, batchLoads );
  }
}

Media::ContainerEntityPtr MediaSetDecompilerImpl::checkCreateDirectory( const std::filesystem::path &directoryPath )
{
  // make path relative (remove leading slash)
  const auto dirPath{ directoryPath.relative_path() };

  // we are in root-directory
  if ( dirPath.empty() )
  {
    return mediaSetV;
  }

  Media::ContainerEntityPtr dir{ mediaSetV };
  assert( dir );

  // iterate over path elements
  for ( const auto &subPath : dirPath )
  {
    auto subDir{ dir->subdirectory( std::string_view{ subPath.string() } ) };

    // if subdirectory does not exist - create it
    if ( !subDir )
    {
      subDir = dir->addSubdirectory( subPath.string() );
      assert( subDir );
    }

    dir = subDir;
  }

  return dir;
}

void MediaSetDecompilerImpl::checkMediumFiles( const MediumNumber &mediumNumber ) const
{
  // iterate over files
  for ( const auto &fileInfo : filesInfosV | std::views::values )
  {
    // skip files, which are not part of the current medium
    if ( fileInfo.memberSequenceNumber != mediumNumber )
    {
      continue;
    }

    // skip file integrity checks if requested
    if ( !checkFileIntegrityV )
    {
      continue;
    }

    checkFileIntegrity( fileInfo );
  }
}

void MediaSetDecompilerImpl::checkFileIntegrity( const Files::FileInfo &fileInfo ) const
{
  SPDLOG_TRACE( "Check file '{}'", fileInfo.path().generic_string() );

  const auto rawFile{ readFileHandlerV( fileInfo.memberSequenceNumber, fileInfo.path() ) };

  // compare checksums
  if (
    const auto crc{ Files::Arinc665File::calculateChecksum( rawFile ) };
    crc != fileInfo.crc )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "CRC of file invalid" }
      << boost::errinfo_file_name{ fileInfo.path().string() } );
  }

  // Check and compare Check Value
  if ( Arinc649::CheckValueType::NotUsed != fileInfo.checkValue.type() )
  {
    const auto checkValueCalculated{ Arinc649::CheckValueGenerator::checkValue(
      fileInfo.checkValue.type(),
      std::as_bytes( Helper::ConstRawDataSpan{ rawFile } ) ) };

    if ( fileInfo.checkValue != checkValueCalculated )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Check Value of file invalid" }
        << boost::errinfo_file_name{ fileInfo.path().string() } );
    }
  }
}

void MediaSetDecompilerImpl::checkLoadFile(
  Arinc649::Arinc649Crc32 &loadCrc,
  Arinc649::CheckValueGenerator &loadCheckValueGenerator,
  const Files::FileInfo &fileInfo,
  const Files::LoadFileInfo &loadFileInfo,
  bool fileSize16Bit ) const
{
  // get memorised file size (only when file integrity is checked)
  if ( checkFileIntegrityV )
  {
    const auto fileSize{ fileSizeHandlerV( fileInfo.memberSequenceNumber, fileInfo.path() ) };

    // check load data file size - we divide by 2 to work around 16-bit size
    // storage within Supplement 2 LUHs (Only Data Files)
    if ( ( fileSize16Bit && ( fileSize / 2 != loadFileInfo.length / 2 ) )
      || ( !fileSize16Bit && ( fileSize != loadFileInfo.length ) ) )
    {
      SPDLOG_ERROR( "Load File Size inconsistent '{}' {} {}", loadFileInfo.filename, fileSize, loadFileInfo.length );

      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Load File Size inconsistent" }
        << boost::errinfo_file_name{ loadFileInfo.filename } );
    }
  }

  // Check CRC
  if ( fileInfo.crc != loadFileInfo.crc )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665Exception()
      << Helper::AdditionalInfo{ "Load File CRC inconsistent" }
      << boost::errinfo_file_name{ loadFileInfo.filename } );
  }

  // Check File Check Value
  const bool fileCheckValueChecked{ checkCheckValues( fileInfo.checkValue, loadFileInfo.checkValue ) };

  // Load CRC, Load Check Value and File Check Value Check
  if ( checkFileIntegrityV )
  {
    const auto rawDataFile{ readFileHandlerV( fileInfo.memberSequenceNumber, fileInfo.path() ) };

    loadCrc.process_bytes( rawDataFile.data(), rawDataFile.size() );
    loadCheckValueGenerator.process( std::as_bytes( Helper::ConstRawDataSpan{ rawDataFile } ) );

    // Load file Check Value
    if ( !fileCheckValueChecked
      && ( Arinc649::CheckValueGenerator::checkValue(
        loadFileInfo.checkValue.type(),
        std::as_bytes( Helper::ConstRawDataSpan{ rawDataFile } ) ) != loadFileInfo.checkValue ) )
    {
      BOOST_THROW_EXCEPTION(
        Arinc665Exception()
          << Helper::AdditionalInfo{ "Load File Check Value inconsistent" }
          << boost::errinfo_file_name{ loadFileInfo.filename } );
    }
  }
}

bool MediaSetDecompilerImpl::checkCheckValues(
  const Arinc649::CheckValue &fileListCheckValue,
  const Arinc649::CheckValue &loadFileCheckValue ) const
{
  if ( loadFileCheckValue.type() == Arinc649::CheckValueType::NotUsed )
  {
    // no load file check value provided
    return true;
  }

  if ( fileListCheckValue.type() == loadFileCheckValue.type() )
  {
    if ( fileListCheckValue != loadFileCheckValue )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Load File Check Value inconsistent" } );
    }

    return true;
  }

  return false;
}

}
