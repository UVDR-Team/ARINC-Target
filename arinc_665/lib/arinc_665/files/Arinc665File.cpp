// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::Arinc665File.
 **/

#include "Arinc665File.hpp"
#include <helper/Underlying.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/Arinc649Crc.hpp>

#include <helper/Exception.hpp>
#include <helper/RawData.hpp>
#include <helper/SafeCast.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

std::string Arinc665File::encodePath( const std::filesystem::path &path )
{
  std::string convertedPath{ path.string() };

  std::ranges::replace( convertedPath, '/', '\\' );

  // append final backslash if not present
  if ( !convertedPath.ends_with( '\\' ) )
  {
    convertedPath += '\\';
  }

  return convertedPath;
}

uint32_t Arinc665File::fileLength( Helper::ConstRawDataSpan file )
{
  // check file size
  if ( file.size() < BaseHeaderSize )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File()
      << Helper::AdditionalInfo{ "file content too small" } );
  }

  // decode the file length
  auto [ _, fileLength ]{ Helper::RawData_getInt< uint32_t >( file.subspan( FileLengthFieldOffset ) ) };

  return fileLength;
}

uint16_t Arinc665File::formatVersion( Helper::ConstRawDataSpan file )
{
  // check file size
  if ( file.size() < BaseHeaderSize )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File{}
      << Helper::AdditionalInfo{ "file content too small" } );
  }

  // decode the format version
  auto [ _, formatVersion ]{ Helper::RawData_getInt< uint16_t >( file.subspan( FileFormatVersionFieldOffset ) ) };

  return formatVersion;
}

uint16_t Arinc665File::calculateChecksum( Helper::ConstRawDataSpan file )
{
  Arinc649::Arinc649Crc16 arincCrc16;

  arincCrc16.process_block( std::data( file ), std::data( file ) + file.size() );

  return arincCrc16.checksum();
}

std::optional< FileClassType > Arinc665File::fileType( Helper::ConstRawDataSpan rawFile )
{
  switch ( formatVersion( rawFile ) )
  {
    case Helper::toUnderlying( LoadFileFormatVersion::Version2 ):
    case Helper::toUnderlying( LoadFileFormatVersion::Version345 ):
      return FileClassType::LoadFile;

    case Helper::toUnderlying( BatchFileFormatVersion::Version2 ):
    case Helper::toUnderlying( BatchFileFormatVersion::Version345 ):
      return FileClassType::BatchFile;

    case Helper::toUnderlying( MediaFileFormatVersion::Version2 ):
    case Helper::toUnderlying( MediaFileFormatVersion::Version345 ):
      return FileClassType::MediaFile;

    default:
      return {};
  }
}

std::optional< LoadFileFormatVersion > Arinc665File::loadFileFormatVersion( Helper::ConstRawDataSpan rawFile )
{
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  const LoadFileFormatVersion formatVersion{ Arinc665File::formatVersion( rawFile ) };

  switch ( formatVersion )
  {
    case LoadFileFormatVersion::Version2:
    case LoadFileFormatVersion::Version345:
      break;

    default:
      return {};
  }

  return formatVersion;
}

std::optional< BatchFileFormatVersion > Arinc665File::batchFileFormatVersion( Helper::ConstRawDataSpan rawFile )
{
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  const BatchFileFormatVersion formatVersion{ Arinc665File::formatVersion( rawFile ) };

  switch ( formatVersion )
  {
    case BatchFileFormatVersion::Version2:
    case BatchFileFormatVersion::Version345:
      break;

    default:
      return {};
  }

  return formatVersion;
}

std::optional< MediaFileFormatVersion > Arinc665File::mediaFileFormatVersion( Helper::ConstRawDataSpan rawFile )
{
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
  const MediaFileFormatVersion formatVersion{ Arinc665File::formatVersion( rawFile ) };

  switch ( formatVersion )
  {
    case MediaFileFormatVersion::Version2:
    case MediaFileFormatVersion::Version345:
      break;

    default:
      return {};
  }

  return formatVersion;
}

std::optional< SupportedArinc665Version > Arinc665File::arinc665Version(
  const FileType fileType,
  const uint16_t formatVersionField ) noexcept
{
  switch ( fileType)
  {
    case FileType::BatchFile:
      // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
      switch ( BatchFileFormatVersion{ formatVersionField } )
      {
        case BatchFileFormatVersion::Version2:
          return SupportedArinc665Version::Supplement2;

        case BatchFileFormatVersion::Version345:
          return SupportedArinc665Version::Supplement345;

        default:
          break;
      }
      break;

    case FileType::LoadUploadHeader:
      // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
      switch ( LoadFileFormatVersion{ formatVersionField } )
      {
        case LoadFileFormatVersion::Version2:
          return SupportedArinc665Version::Supplement2;

        case LoadFileFormatVersion::Version345:
          return SupportedArinc665Version::Supplement345;

        default:
          break;
      }
      break;

    case FileType::LoadList:
    case FileType::BatchList:
    case FileType::FileList:
      // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Check for validity
      switch ( MediaFileFormatVersion{ formatVersionField } )
      {
        case MediaFileFormatVersion::Version2:
          return SupportedArinc665Version::Supplement2;

        case MediaFileFormatVersion::Version345:
          return SupportedArinc665Version::Supplement345;

        default:
          break;
      }
      break;

    default:
      break;
  }

  return {};
}

uint16_t Arinc665File::formatVersionField(
  const FileType fileType,
  const SupportedArinc665Version arinc665Version ) noexcept
{
  switch ( fileType)
  {
    case FileType::BatchFile:
      switch ( arinc665Version )
      {
        case SupportedArinc665Version::Supplement2:
          return Helper::toUnderlying( BatchFileFormatVersion::Version2 );

        case SupportedArinc665Version::Supplement345:
          return Helper::toUnderlying( BatchFileFormatVersion::Version345 );

        default:
          break;
      }
      break;

    case FileType::LoadUploadHeader:
      switch ( arinc665Version )
      {
        case SupportedArinc665Version::Supplement2:
          return Helper::toUnderlying( LoadFileFormatVersion::Version2 );

        case SupportedArinc665Version::Supplement345:
          return Helper::toUnderlying( LoadFileFormatVersion::Version345 );

        default:
          break;
      }
      break;

    case FileType::LoadList:
    case FileType::BatchList:
    case FileType::FileList:
      switch ( arinc665Version )
      {
        case SupportedArinc665Version::Supplement2:
          return Helper::toUnderlying( MediaFileFormatVersion::Version2 );

        case SupportedArinc665Version::Supplement345:
          return Helper::toUnderlying( MediaFileFormatVersion::Version345 );

        default:
          break;
      }
      break;

    default:
      break;
  }

  return 0xFFFFU;
}

std::optional< FileType > Arinc665File::fileType( const std::filesystem::path &filename )
{
  const auto filenameN{ filename.filename().string() };

  if ( filenameN == ListOfLoadsName )
  {
    return FileType::LoadList;
  }

  if ( filenameN == ListOfBatchesName )
  {
    return FileType::BatchList;
  }

  if ( filenameN == ListOfFilesName )
  {
    return FileType::FileList;
  }

  const auto extension{ filename.extension().string() };

  if ( extension == LoadUploadHeaderExtension )
  {
    return FileType::LoadUploadHeader;
  }

  if ( extension == BatchFileExtension )
  {
    return FileType::BatchFile;
  }

  return {};
}

Arinc665File& Arinc665File::operator=( Helper::ConstRawDataSpan rawFile )
{
  decodeHeader( rawFile, fileType() );
  return *this;
}

Arinc665File::operator Helper::RawData() const
{
  return encode();
}

SupportedArinc665Version Arinc665File::arincVersion() const noexcept
{
  return arinc665VersionV;
}

void Arinc665File::arincVersion( const SupportedArinc665Version version ) noexcept
{
  arinc665VersionV = version;
}

Arinc665File::Arinc665File( const SupportedArinc665Version version, const ptrdiff_t checksumPosition ) noexcept :
  checksumPosition{ checksumPosition },
  arinc665VersionV{ version }
{
}

Arinc665File::Arinc665File(
  Helper::ConstRawDataSpan rawFile,
  const FileType expectedFileType,
  const ptrdiff_t checksumPosition ) :
  checksumPosition{ checksumPosition },
  arinc665VersionV{ SupportedArinc665Version::Supplement2 }
{
  decodeHeader( rawFile, expectedFileType );
}

Arinc665File& Arinc665File::operator=( const Arinc665File &other ) noexcept
{
  if ( this == &other)
  {
    return *this;
  }

  // if this assertion fails, we have an error within this library
  assert( this->checksumPosition == other.checksumPosition );

  arinc665VersionV = other.arinc665VersionV;
  return *this;
}

Arinc665File& Arinc665File::operator=( Arinc665File &&other ) noexcept
{
  if ( this == &other )
  {
    return *this;
  }

  // if this assertion fails, we have an error within this library
  assert( this->checksumPosition == other.checksumPosition );

  arinc665VersionV = other.arinc665VersionV;
  return *this;
}

void Arinc665File::insertHeader( Helper::RawDataSpan rawFile, const std::size_t additionalSize ) const
{
  const auto fileSize{ rawFile.size() + additionalSize };

  // Check file size
  if ( rawFile.size() <= ( BaseHeaderSize + sizeof( uint16_t ) ) )
  {
    BOOST_THROW_EXCEPTION(
      InvalidArinc665File{} << Helper::AdditionalInfo{ "File too small" } );
  }

  // Check file size
  if ( fileSize % 2 != 0U )
  {
    BOOST_THROW_EXCEPTION(
      InvalidArinc665File{} << Helper::AdditionalInfo{ "Invalid size" } );
  }

  // file size
  Helper::RawData_setInt( rawFile.subspan( FileLengthFieldOffset ), Helper::safeCast< uint32_t >( fileSize / 2U ) );

  // format version
  Helper::RawData_setInt< uint16_t >(
    rawFile.subspan( FileFormatVersionFieldOffset ),
    formatVersionField( fileType(), arinc665VersionV ) );
}

void Arinc665File::calculateFileCrc( Helper::RawDataSpan rawFile ) const
{
  // calculate crc
  const auto calculatedCrc{ calculateChecksum( rawFile.first( rawFile.size() - checksumPosition ) ) };

  // set crc field
  Helper::RawData_setInt< uint16_t >( rawFile.last( checksumPosition ), calculatedCrc );
}

void Arinc665File::decodeHeader( Helper::ConstRawDataSpan rawFile, const FileType expectedFileType )
{
  // Check file size
  if ( rawFile.size() <= BaseHeaderSize )
  {
    BOOST_THROW_EXCEPTION(
      InvalidArinc665File{} << Helper::AdditionalInfo{ "File to small" } );
  }

  // check size field
  auto [ _, fileLength ]{ Helper::RawData_getInt< uint32_t >( rawFile.subspan( FileLengthFieldOffset ) ) };

  if ( static_cast< size_t >( fileLength ) * 2U != rawFile.size() )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "file size invalid" } );
  }

  // format version
  auto [ _1, formatVersion ]{ Helper::RawData_getInt< uint16_t >( rawFile.subspan( FileFormatVersionFieldOffset ) ) };

  const auto optionalArinc665Version{ arinc665Version( expectedFileType, formatVersion ) };

  // check format field version
  if ( !optionalArinc665Version )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "wrong file format" } );
  }

  arinc665VersionV = *optionalArinc665Version;

  // Decode checksum field
  auto [ _2, crc ]{ Helper::RawData_getInt< uint16_t >( rawFile.last( checksumPosition ) ) };

  // calculate checksum and compare against stored
  const auto calcCrc{ calculateChecksum( rawFile.first( rawFile.size() - checksumPosition ) ) };
  if ( crc != calcCrc )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "Invalid checksum" } );
  }
}

}
