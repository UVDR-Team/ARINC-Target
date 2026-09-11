// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::FileListFile.
 **/

#include "FileListFile.hpp"

#include <arinc_665/files/StringUtils.hpp>
#include <arinc_665/files/CheckValueUtils.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/CheckValueGenerator.hpp>

#include <helper/Exception.hpp>
#include <helper/RawData.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

FileListFile::FileListFile( const SupportedArinc665Version version ) :
  ListFile{ version }
{
}

FileListFile::FileListFile( Helper::ConstRawDataSpan rawFile ) :
  ListFile{ rawFile, FileType::FileList }
{
  decodeBody( rawFile );
}

FileListFile& FileListFile::operator=( Helper::ConstRawDataSpan rawFile )
{
  Arinc665File::operator =( rawFile );
  decodeBody( rawFile );

  return *this;
}

FileType FileListFile::fileType() const noexcept
{
  return FileType::FileList;
}

size_t FileListFile::numberOfFiles() const
{
  return filesV.size();
}

const FilesInfo& FileListFile::files() const
{
  return filesV;
}

FilesInfo& FileListFile::files()
{
  return filesV;
}

void FileListFile::files( FilesInfo files )
{
  filesV = std::move( files );
}

void FileListFile::file( FileInfo file )
{
  filesV.push_back( std::move( file ) );
}

Helper::ConstRawDataSpan FileListFile::userDefinedData() const
{
  return userDefinedDataV;
}

void FileListFile::userDefinedData( Helper::RawData userDefinedData )
{
  userDefinedDataV = std::move( userDefinedData );

  checkUserDefinedData();
}

Arinc649::CheckValueType FileListFile::checkValueType() const
{
  return checkValueTypeV;
}

void FileListFile::checkValueType( const Arinc649::CheckValueType type )
{
  checkValueTypeV = type;
}

bool FileListFile::belongsToSameMediaSet( const FileListFile &other ) const
{
  //! @todo rework

  if ( ( mediaSetPn() != other.mediaSetPn() )
    || ( numberOfMediaSetMembers() != other.numberOfMediaSetMembers() )
    || !std::ranges::equal( userDefinedDataV, other.userDefinedData() ) )
  {
    return false;
  }

  const auto &otherFileList( other.files() );

  if ( filesV.size() != otherFileList.size() )
  {
    return false;
  }

  auto fileIt{ filesV.begin() };
  auto otherFileIt{ otherFileList.begin() };
  for ( size_t i = 0; i < filesV.size(); ++i )
  {
    if ( ( fileIt->filename != otherFileIt->filename )
      || ( fileIt->pathName != otherFileIt->pathName ) )
    {
      return false;
    }

    if ( const auto fileType{ Arinc665File::fileType( fileIt->filename ) }; fileType )
    {
      switch ( *fileType )
      {
        case FileType::LoadList:
        case FileType::BatchList:
          // skip test of CRC and Member Sequence Number
          break;

        default:
          if ( ( fileIt->crc != otherFileIt->crc )
            || ( fileIt->checkValue != otherFileIt->checkValue )
            || ( fileIt->memberSequenceNumber != otherFileIt->memberSequenceNumber ) )
          {
            return false;
          }

          break;
      }
    }
    else
    {
      if ( ( fileIt->crc != otherFileIt->crc )
        || ( fileIt->checkValue != otherFileIt->checkValue )
        || ( fileIt->memberSequenceNumber != otherFileIt->memberSequenceNumber ) )
      {
        return false;
      }
    }

    ++fileIt;
    ++otherFileIt;
  }

  return true;
}

Helper::RawData FileListFile::encode() const
{
  bool encodeV3Data{ false };
  std::size_t baseSize;

  switch ( arincVersion() )
  {
    case SupportedArinc665Version::Supplement2:
      baseSize = FileHeaderSizeV2;
      break;

    case SupportedArinc665Version::Supplement345:
      encodeV3Data = true;
      baseSize = FileHeaderSizeV3;
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "Unsupported ARINC 665 Version" } );
  }

  Helper::RawData rawFile( baseSize );

  // spare field
  Helper::RawData_setInt< uint16_t>( Helper::RawDataSpan{ rawFile }.subspan( SpareFieldOffsetV2 ), 0U );

  // Next free Offset (used for optional pointer calculation)
  ptrdiff_t nextFreeOffset{ static_cast< ptrdiff_t>( rawFile.size() ) };


  // media set information
  const auto rawMediaInformation{ encodeMediaInformation() };
  assert( rawMediaInformation.size() % 2 == 0 );
  rawFile.insert( rawFile.end(), rawMediaInformation.begin(), rawMediaInformation.end() );

  // Update Pointer
  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan( MediaSetPartNumberPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2U ) );
  nextFreeOffset += static_cast< ptrdiff_t >( rawMediaInformation.size() );


  // media set files list
  const auto rawFilesInfo{ encodeFilesInfo( encodeV3Data ) };
  assert( rawFilesInfo.size() % 2 == 0 );

  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan( MediaSetFilesPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2U ) );
  nextFreeOffset += static_cast< ptrdiff_t>( rawFilesInfo.size() );

  rawFile.insert( rawFile.end(), rawFilesInfo.begin(), rawFilesInfo.end() );

  // user defined data
  assert( userDefinedDataV.size() % 2 == 0);
  uint32_t userDefinedDataPtr = 0;

  if ( !userDefinedDataV.empty() )
  {
    userDefinedDataPtr = static_cast< uint32_t >( nextFreeOffset / 2U );
    nextFreeOffset += static_cast< ptrdiff_t>( userDefinedDataV.size() );

    rawFile.insert( rawFile.end(), userDefinedDataV.begin(), userDefinedDataV.end() );
  }

  // update User Defined Data Pointer
  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan( UserDefinedDataPointerFieldOffsetV2 ),
    userDefinedDataPtr );


  // Update Check Value Pointer (only in V3 mode)
  // must be done before check value and CRC generation
  size_t checkValueSize{ 0U };
  if ( encodeV3Data )
  {
    // Check Value Pointer
    Helper::RawData_setInt< uint32_t >(
      Helper::RawDataSpan{ rawFile }.subspan( FileCheckValuePointerFieldOffsetV3 ),
      static_cast< uint32_t >( nextFreeOffset / 2 ) );

    // Update size
    checkValueSize = CheckValueUtils_size( checkValueTypeV );
  }

  // set header ( Check Value and File CRC must be added to size field
  // must be done before check value and CRC generation
  insertHeader( rawFile, checkValueSize + sizeof( uint16_t ) );

  // Check Value (only in V3 mode)
  if ( encodeV3Data )
  {
    // calculate and encode File Load Check Value
    const auto rawCheckValue{ CheckValueUtils_encode(
      Arinc649::CheckValueGenerator::checkValue(
        checkValueTypeV,
        std::as_bytes( Helper::ConstRawDataSpan{ rawFile } ) ).value_or( Arinc649::CheckValue::NoCheckValue ) ) };
    assert( rawCheckValue.size() % 2 == 0 );

    rawFile.insert( rawFile.end(), rawCheckValue.begin(), rawCheckValue.end() );
  }

  // Resize to final size ( File CRC)
  rawFile.resize( rawFile.size() + sizeof( uint16_t ) );

  // set CRC
  calculateFileCrc( rawFile );

  return rawFile;
}

void FileListFile::decodeBody( Helper::ConstRawDataSpan rawFile )
{
  bool decodeV3Data{ false };

  switch ( arincVersion() )
  {
    case SupportedArinc665Version::Supplement2:
      break;

    case SupportedArinc665Version::Supplement345:
      decodeV3Data = true;
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "Unsupported ARINC 665 Version" } );
  }


  // Spare Field
  uint16_t spare{};
  std::tie( std::ignore, spare ) = Helper::RawData_getInt< uint16_t >( rawFile.subspan( SpareFieldOffsetV2 ) );

  if ( 0U != spare )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File()
      << Helper::AdditionalInfo{ "Spare is not 0" } );
  }

  // media information pointer
  uint32_t mediaInformationPtr{};
  std::tie( std::ignore, mediaInformationPtr ) =
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( MediaSetPartNumberPointerFieldOffsetV2 ) );

  // file list pointer
  uint32_t fileListPtr{};
  std::tie( std::ignore, fileListPtr ) =
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( MediaSetFilesPointerFieldOffsetV2 ) );

  // user defined data pointer
  uint32_t userDefinedDataPtr{};
  std::tie( std::ignore, userDefinedDataPtr ) =
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( UserDefinedDataPointerFieldOffsetV2) );

  uint32_t fileCheckValuePtr{};

  // only decode this pointers in V3 mode
  if ( decodeV3Data )
  {
    std::tie( std::ignore, fileCheckValuePtr ) =
      Helper::RawData_getInt< uint32_t >( rawFile.subspan( FileCheckValuePointerFieldOffsetV3 ) );
  }

  // decode Media Information
  decodeMediaInformation( rawFile.subspan( 2ULL * mediaInformationPtr ) );

  // file list
  decodeFilesInfo( rawFile.subspan( fileListPtr * 2ULL ), decodeV3Data );

  // user defined data
  if ( 0 != userDefinedDataPtr )
  {
    ptrdiff_t endOfUserDefinedData{ static_cast< ptrdiff_t>( rawFile.size() ) - DefaultChecksumPosition };

    if ( fileCheckValuePtr != 0 )
    {
      if ( fileCheckValuePtr <= userDefinedDataPtr )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "Invalid Pointers" } );
      }

      endOfUserDefinedData = static_cast< ptrdiff_t >( fileCheckValuePtr ) * 2;
    }

    userDefinedDataV.assign(
      rawFile.begin() + static_cast< ptrdiff_t >( userDefinedDataPtr ) * 2,
      rawFile.begin() + endOfUserDefinedData );
  }

  // File Check Value Field (ARINC 665-3)
  checkValueTypeV = Arinc649::CheckValueType::NotUsed;
  if ( decodeV3Data && ( 0U != fileCheckValuePtr ) )
  {
    const auto checkValue{
      CheckValueUtils_decode( rawFile.subspan( 2U * static_cast< size_t >( fileCheckValuePtr ) ) ) };

    checkValueTypeV = checkValue.type();

    if ( Arinc649::CheckValueType::NotUsed != checkValueTypeV )
    {
      // calculate Check Value
      const auto calcCheckValue{ Arinc649::CheckValueGenerator::checkValue(
        checkValueTypeV,
        std::as_bytes( rawFile.first( 2U * static_cast< std::size_t >( fileCheckValuePtr ) ) ) ) };

      if ( checkValue != calcCheckValue )
      {
        BOOST_THROW_EXCEPTION(
          InvalidArinc665File{} << Helper::AdditionalInfo{ "Check Value Verification failed" } );
      }
    }
  }

  // file crc decoded and checked within base class
}

Helper::RawData FileListFile::encodeFilesInfo( const bool encodeV3Data ) const
{
  Helper::RawData rawFilesInfo( sizeof( uint16_t ) );

  // Number of files must not exceed field
  if ( filesV.size() > std::numeric_limits< uint16_t>::max() )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File()
      << Helper::AdditionalInfo{ "More files than allowed" } );
  }

  // number of files
  Helper::RawData_setInt< uint16_t >( rawFilesInfo, static_cast< uint16_t >( filesV.size() ) );

  // iterate over files
  uint16_t fileCounter{ 0 };
  for ( auto const &fileInfo : filesV )
  {
    ++fileCounter;

    Helper::RawData rawFileInfo( sizeof( uint16_t) );

    // filename
    auto const rawFilename{ StringUtils_encodeString( fileInfo.filename ) };
    assert( rawFilename.size() % 2 == 0 );
    rawFileInfo.insert( rawFileInfo.end(), rawFilename.begin(), rawFilename.end() );

    // path name
    auto const rawPathname{ StringUtils_encodeString( fileInfo.pathName ) };
    assert( rawPathname.size() % 2 == 0);
    rawFileInfo.insert( rawFileInfo.end(), rawPathname.begin(), rawPathname.end() );

    rawFileInfo.resize( rawFileInfo.size() + ( 2 * sizeof( uint16_t ) ) );

    // member sequence number
    auto next{ Helper::RawData_setInt< uint16_t>(
      Helper::RawDataSpan{ rawFileInfo }.last( 2 * sizeof( uint16_t ) ),
      static_cast< uint8_t >( fileInfo.memberSequenceNumber ) ) };

    // crc
    Helper::RawData_setInt< uint16_t>( next, fileInfo.crc );

    // following fields are available in ARINC 665-3 ff
    if ( encodeV3Data )
    {
      // check Value
      const auto rawCheckValue{ CheckValueUtils_encode( fileInfo.checkValue ) };
      assert( rawCheckValue.size() % 2 == 0 );
      rawFileInfo.insert( rawFileInfo.end(), rawCheckValue.begin(), rawCheckValue.end() );
    }

    // next file pointer (is set to 0 for last file)
    Helper::RawData_setInt< uint16_t >(
      rawFileInfo,
      ( fileCounter == numberOfFiles() ) ? 0U : static_cast< uint16_t >( rawFileInfo.size() / 2U ) );

    // add file info to files info
    rawFilesInfo.insert( rawFilesInfo.end(), rawFileInfo.begin(), rawFileInfo.end() );
  }

  return rawFilesInfo;
}

void FileListFile::decodeFilesInfo( Helper::ConstRawDataSpan rawData, const bool decodeV3Data )
{
  auto remaining{ rawData };

  // clear potentially data
  filesV.clear();

  // number of files
  uint16_t numberOfFiles{};
  std::tie( remaining, numberOfFiles ) = Helper::RawData_getInt< uint16_t>( remaining );

  // iterate over index
  for ( unsigned int fileIndex = 0U; fileIndex < numberOfFiles; ++fileIndex )
  {
    auto listRemaining{ remaining };

    // next file pointer
    uint16_t filePointer{};
    std::tie( listRemaining, filePointer ) = Helper::RawData_getInt< uint16_t >( listRemaining );

    // check file pointer for validity
    if ( fileIndex != numberOfFiles - 1U )
    {
      if ( filePointer == 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "next file pointer is 0" } );
      }
    }
    else
    {
      if ( filePointer != 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "next file pointer is not 0" } );
      }
    }

    // filename
    std::string filename;
    std::tie( listRemaining, filename ) = StringUtils_decodeString( listRemaining );

    // path name
    std::string pathName;
    std::tie( listRemaining, pathName ) = StringUtils_decodeString( listRemaining );

    // member sequence number
    uint16_t memberSequenceNumber;
    std::tie( listRemaining, memberSequenceNumber ) = Helper::RawData_getInt< uint16_t >( listRemaining );
    if ( ( memberSequenceNumber < 1U ) || ( memberSequenceNumber > 255U ) )
    {
      BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "member sequence number out of range" } );
    }

    // crc
    uint16_t crc;
    std::tie( listRemaining, crc ) = Helper::RawData_getInt< uint16_t >( listRemaining );

    // CheckValue (keep default initialised if not V3 File Info
    Arinc649::CheckValue checkValue{ Arinc649::CheckValue::NoCheckValue };

    // following fields are available in ARINC 665-3 ff
    if ( decodeV3Data )
    {
      // check Value
      checkValue = CheckValueUtils_decode( listRemaining );
    }

    // set it to begin of next file
    remaining = remaining.subspan( filePointer * 2ULL );

    filesV.emplace_back( FileInfo{
      .filename = std::move( filename ),
      .pathName = std::move( pathName ),
      .memberSequenceNumber = MediumNumber{ static_cast< uint8_t >( memberSequenceNumber ) },
      .crc = crc,
      .checkValue = checkValue } );
  }
}

void FileListFile::checkUserDefinedData()
{
  if ( userDefinedDataV.size() % 2U != 0U )
  {
    SPDLOG_WARN( "User defined data must be 2-byte aligned. - extending range" );

    userDefinedDataV.push_back( std::byte{ 0U } );
  }
}

}
