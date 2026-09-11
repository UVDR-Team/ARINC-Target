// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::LoadDecompilerImpl.
 **/

#include "LoadDecompilerImpl.hpp"

#include <arinc_665/media/ContainerEntity.hpp>
#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <arinc_665/files/LoadHeaderFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/CheckValueGenerator.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc665::Utils {

LoadDecompiler& LoadDecompilerImpl::filePathHandler( FilePathHandler handler )
{
  filePathHandlerV = std::move( handler );
  return *this;
}

LoadDecompiler& LoadDecompilerImpl::fileSizeHandler( FileSizeHandler handler )
{
  fileSizeHandlerV = std::move( handler );
  return *this;
}

LoadDecompiler& LoadDecompilerImpl::readFileHandler( ReadFileHandler handler )
{
  readFileHandlerV = std::move( handler );
  return *this;
}

LoadDecompiler& LoadDecompilerImpl::checkFileIntegrity( const bool checkFileIntegrity ) noexcept
{
  checkFileIntegrityV = checkFileIntegrity;
  return *this;
}

LoadDecompiler& LoadDecompilerImpl::loadFilename( std::string_view filename )
{
  loadFilenameV = std::string( filename );
  return *this;
}

LoadDecompilerResult LoadDecompilerImpl::operator()()
{
  checkValuesV.clear();

  mediaSetV = Media::MediaSet::create();
  loadFile();

  // Update Media Set
  mediaSetV->partNumber( std::string{ loadV->partNumber() } );

  return { loadV, mediaSetV, checkValuesV };
}

void LoadDecompilerImpl::loadFile()
{
  // add load to media set
  loadV = mediaSetV->addLoad( loadFilenameV );
  assert( loadV );

  // decode load header
  const auto rawLoadHeaderFile{ readFileHandlerV( loadFilenameV ) };
  Files::LoadHeaderFile loadHeaderFile{ rawLoadHeaderFile };

  loadV->partFlags( loadHeaderFile.partFlags() );
  loadV->partNumber( std::string{ loadHeaderFile.partNumber() } );
  loadV->loadType( loadHeaderFile.loadType() );
  Media::Load::TargetHardwareIdPositions thwIdsPositions{};
  for ( const auto &[ thwId, positions ] : loadHeaderFile.targetHardwareIdsPositions() )
  {
    thwIdsPositions.try_emplace( thwId, positions.begin(), positions.end() );
  }
  for ( const auto &thwId : loadHeaderFile.targetHardwareIds() )
  {
    // if not previously added, add it now with empty positions
    thwIdsPositions.try_emplace( thwId );
  }
  loadV->targetHardwareIdPositions( std::move( thwIdsPositions ) );

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
    // filepath
    auto filePath{ filePathHandlerV( loadFileInfo.filename ) };

    const auto fileDir{ checkCreateDirectory( filePath.parent_path() ) };

    const auto dataFilePtr{ fileDir->addRegularFile( loadFileInfo.filename ) };

    // perform file check
    // in ARINC 665-2 File Size of Data File is stored as multiple of 16 bit
    checkLoadFile(
      loadCrc,
      *loadCheckValueGenerator,
      filePath,
      loadFileInfo,
      loadHeaderFile.arincVersion() == SupportedArinc665Version::Supplement2 );

    loadV->dataFile( dataFilePtr, loadFileInfo.partNumber, loadFileInfo.checkValue.type() );

    checkValuesV[ dataFilePtr ].emplace(( Arinc649::CheckValue::crc16( loadFileInfo.crc ) ) );

    // Add check value if provided
    if ( Arinc649::CheckValue::NoCheckValue != loadFileInfo.checkValue )
    {
      checkValuesV[ dataFilePtr ].emplace( loadFileInfo.checkValue );
    }
  }

  // iterate over support files
  for ( const auto &loadFileInfo : loadHeaderFile.supportFiles() )
  {
    // filepath
    auto filePath{ filePathHandlerV( loadFileInfo.filename ) };

    const auto fileDir{ checkCreateDirectory( filePath.parent_path() ) };

    // start search in parent directory of load (according ARINC 665-5)
    auto supportFilePtr{ fileDir->addRegularFile( loadFileInfo.filename ) };

    checkLoadFile( loadCrc, *loadCheckValueGenerator, filePath, loadFileInfo, false );

    loadV->supportFile( supportFilePtr, loadFileInfo.partNumber, loadFileInfo.checkValue.type() );

    checkValuesV[ supportFilePtr ].emplace(( Arinc649::CheckValue::crc16( loadFileInfo.crc ) ) );

    // Add check value if provided
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
        << boost::errinfo_file_name{ loadFilenameV } );
    }

    if ( Files::LoadHeaderFile::decodeLoadCheckValue( rawLoadHeaderFile ) != loadCheckValueGenerator->checkValue() )
    {
      BOOST_THROW_EXCEPTION(
        Arinc665Exception()
        << Helper::AdditionalInfo{ "Load Check Value inconsistent" }
        << boost::errinfo_file_name{ loadFilenameV } );
    }
  }

  // User Defined Data
  auto loadUserDefinedData{ loadHeaderFile.userDefinedData() };
  loadV->userDefinedData( Helper::RawData{ loadUserDefinedData.begin(), loadUserDefinedData.end() } );
  // Load Check Value
  loadV->loadCheckValueType( loadHeaderFile.loadCheckValueType() );
}

Media::ContainerEntityPtr LoadDecompilerImpl::checkCreateDirectory( const std::filesystem::path &directoryPath )
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

void LoadDecompilerImpl::checkLoadFile(
  Arinc649::Arinc649Crc32 &loadCrc,
  Arinc649::CheckValueGenerator &loadCheckValueGenerator,
  const std::filesystem::path &filePath,
  const Files::LoadFileInfo &loadFileInfo,
  const bool fileSize16Bit ) const
{
  // get memorised file size (only when file integrity is checked)
  if ( checkFileIntegrityV )
  {
    const auto fileSize{ fileSizeHandlerV( filePath ) };

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

  // Load CRC, Load Check Value and File Check Value Check
  if ( checkFileIntegrityV )
  {
    const auto rawDataFile{ readFileHandlerV( filePath ) };

    loadCrc.process_bytes( rawDataFile.data(), rawDataFile.size() );
    loadCheckValueGenerator.process( std::as_bytes( Helper::ConstRawDataSpan{ rawDataFile } ) );

    // Load file Check Value
    if ( Arinc649::CheckValueGenerator::checkValue(
        loadFileInfo.checkValue.type(),
        std::as_bytes( Helper::ConstRawDataSpan{ rawDataFile } ) ) != loadFileInfo.checkValue )
    {
      BOOST_THROW_EXCEPTION(
        Arinc665Exception()
          << Helper::AdditionalInfo{ "Load File Check Value inconsistent" }
          << boost::errinfo_file_name{ loadFileInfo.filename } );
    }
  }
}

}
