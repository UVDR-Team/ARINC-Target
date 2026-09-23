// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Files::LoadHeaderFile.
 **/

#include "LoadHeaderFile.hpp"

#include <arinc_665/files/CheckValueUtils.hpp>
#include <arinc_665/files/StringUtils.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_checksum/CheckValueGenerator.hpp>

#include <arinc_support/Exception.hpp>
#include <arinc_support/SafeCast.hpp>

#include <arinc_support/Logging.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

void LoadHeaderFile::processLoadCrc( ArincSupport::ConstRawDataSpan rawFile, ArincChecksum::Arinc645Crc32 &loadCrc )
{
  loadCrc.process_bytes( std::data( rawFile ), rawFile.size() - LoadCrcOffset );
}

void LoadHeaderFile::encodeLoadCrc( ArincSupport::RawDataSpan rawFile, const uint32_t crc )
{
  ArincSupport::RawData_setInt< uint32_t >( rawFile.last( LoadCrcOffset ), crc );
}

uint32_t LoadHeaderFile::decodeLoadCrc( ArincSupport::ConstRawDataSpan rawFile )
{
  auto [ _, crc ]{ ArincSupport::RawData_getInt< uint32_t >( rawFile.last( LoadCrcOffset ) ) };
  return crc;
}

void LoadHeaderFile::processLoadCheckValue(
  ArincSupport::ConstRawDataSpan rawFile,
  ArincChecksum::CheckValueGenerator &checkValueGenerator )
{
  if ( loadFileFormatVersion( rawFile ) != LoadFileFormatVersion::Version345 )
  {
    // load check value can only be processed on ARINC 665-3/4/5 files
    return;
  }

  // Obtain Load Check Value Pointer
  auto [ _, loadCheckValuePtr ]{
    ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( LoadCheckValuePointerFieldOffsetV3 ) ) };

  if ( 0U == loadCheckValuePtr )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{} << ArincSupport::AdditionalInfo{ "Load Check Value Ptr invalid" } );
  }

  // process check value
  checkValueGenerator.process( rawFile.first( static_cast< size_t >( loadCheckValuePtr ) * 2U ) );
}

void LoadHeaderFile::encodeLoadCheckValue( ArincSupport::RawDataSpan rawFile, const ArincChecksum::CheckValue &checkValue )
{
  if ( loadFileFormatVersion( rawFile ) != LoadFileFormatVersion::Version345 )
  {
    // load check value can only be stored on ARINC 665-3/4/5 files
    return;
  }

  // Obtain Load Check Value Pointer
  auto [ _, loadCheckValuePtr ]{
    ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( LoadCheckValuePointerFieldOffsetV3 ) ) };

  if ( 0U == loadCheckValuePtr )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{} << ArincSupport::AdditionalInfo{ "Load Check Value Ptr invalid" } );
  }

  // write load check value to position
  std::ranges::copy(
    CheckValueUtils_encode( checkValue ),
    rawFile.begin() + static_cast< ptrdiff_t >( loadCheckValuePtr ) * 2 );

  // Update File CRC, which is also calculated over Load Check Value
  const auto calculatedCrc{ calculateChecksum( rawFile.first( rawFile.size() - FileCrcOffset ) ) };

  ArincSupport::RawData_setInt< uint16_t >( rawFile.last( FileCrcOffset ), calculatedCrc );
}

ArincChecksum::CheckValue LoadHeaderFile::decodeLoadCheckValue( ArincSupport::ConstRawDataSpan rawFile )
{
  if ( loadFileFormatVersion( rawFile ) != LoadFileFormatVersion::Version345 )
  {
    return ArincChecksum::CheckValue::NoCheckValue;
  }

  auto [ _, loadCheckValuePtr ]{
    ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( LoadCheckValuePointerFieldOffsetV3 ) ) };

  if ( 0U == loadCheckValuePtr )
  {
    return ArincChecksum::CheckValue::NoCheckValue;
  }

  return CheckValueUtils_decode( rawFile.subspan( static_cast< size_t >( loadCheckValuePtr ) * 2U ) );
}

LoadHeaderFile::LoadHeaderFile( const SupportedArinc665Version version ) :
  Arinc665File{ version, FileCrcOffset }
{
}

LoadHeaderFile::LoadHeaderFile( ArincSupport::ConstRawDataSpan rawFile ) :
  Arinc665File{ rawFile, FileType::LoadUploadHeader, FileCrcOffset }
{
  decodeBody( rawFile );
}

LoadHeaderFile& LoadHeaderFile::operator=( ArincSupport::ConstRawDataSpan rawFile )
{
  // call inherited operator
  Arinc665File::operator =( rawFile );
  decodeBody( rawFile );
  return *this;
}

FileType LoadHeaderFile::fileType() const noexcept
{
  return FileType::LoadUploadHeader;
}

uint16_t LoadHeaderFile::partFlags() const noexcept
{
  return partFlagsV;
}

void LoadHeaderFile::partFlags( uint16_t partFlags ) noexcept
{
  partFlagsV = partFlags;
}

std::string_view LoadHeaderFile::partNumber() const
{
  return partNumberV;
}

void LoadHeaderFile::partNumber( std::string partNumber )
{
  partNumberV = std::move( partNumber );
}

const LoadHeaderFile::TargetHardwareIds& LoadHeaderFile::targetHardwareIds() const
{
  return targetHardwareIdsV;
}

LoadHeaderFile::TargetHardwareIds& LoadHeaderFile::targetHardwareIds()
{
  return targetHardwareIdsV;
}

void LoadHeaderFile::targetHardwareIds( TargetHardwareIds targetHardwareIds )
{
  targetHardwareIdsV = std::move( targetHardwareIds );
}

void LoadHeaderFile::targetHardwareId( std::string targetHardwareId )
{
  targetHardwareIdsV.emplace_back( std::move( targetHardwareId ) );
}

const LoadHeaderFile::TargetHardwareIdsPositions& LoadHeaderFile::targetHardwareIdsPositions() const
{
  return targetHardwareIdsPositionsV;
}

LoadHeaderFile::TargetHardwareIdsPositions& LoadHeaderFile::targetHardwareIdsPositions()
{
  return targetHardwareIdsPositionsV;
}

void LoadHeaderFile::targetHardwareIdsPositions( TargetHardwareIdsPositions targetHardwareIdsPositions )
{
  targetHardwareIdsPositionsV = std::move( targetHardwareIdsPositions );
}

void LoadHeaderFile::targetHardwareIdPositions( std::string targetHardwareId, Positions positions )
{
  targetHardwareIdsPositionsV.emplace_back( std::move( targetHardwareId ), std::move( positions ) );
}

const LoadHeaderFile::LoadType& LoadHeaderFile::loadType() const
{
  return typeV;
}

void LoadHeaderFile::loadType( LoadType type )
{
  typeV = std::move( type );
}

const LoadFilesInfo& LoadHeaderFile::dataFiles() const noexcept
{
  return dataFilesV;
}

LoadFilesInfo& LoadHeaderFile::dataFiles() noexcept
{
  return dataFilesV;
}

void LoadHeaderFile::dataFiles( LoadFilesInfo filesInformation )
{
  dataFilesV = std::move( filesInformation );
}

void LoadHeaderFile::dataFile( LoadFileInfo dataFileInfo )
{
  dataFilesV.push_back( std::move( dataFileInfo ) );
}

const LoadFilesInfo& LoadHeaderFile::supportFiles() const noexcept
{
  return supportFilesV;
}

LoadFilesInfo& LoadHeaderFile::supportFiles() noexcept
{
  return supportFilesV;
}

void LoadHeaderFile::supportFiles( LoadFilesInfo filesInformation )
{
  supportFilesV = std::move( filesInformation );
}

void LoadHeaderFile::supportFile( LoadFileInfo supportFileInfo )
{
  supportFilesV.push_back( std::move( supportFileInfo ) );
}

ArincSupport::ConstRawDataSpan LoadHeaderFile::userDefinedData() const
{
  return userDefinedDataV;
}

void LoadHeaderFile::userDefinedData( ArincSupport::RawData userDefinedData )
{
  userDefinedDataV = std::move( userDefinedData );

  checkUserDefinedData();
}

ArincChecksum::CheckValueType LoadHeaderFile::loadCheckValueType() const
{
  return loadCheckValueTypeV;
}

void LoadHeaderFile::loadCheckValueType( const ArincChecksum::CheckValueType type )
{
  loadCheckValueTypeV = type;
}

ArincSupport::RawData LoadHeaderFile::encode() const
{
  bool encodeV3Data{ false };
  std::size_t baseSize;

  switch ( arincVersion() )
  {
    case SupportedArinc665Version::Supplement2:
      // Spare
      baseSize = LoadHeaderSizeV2;
      break;

    case SupportedArinc665Version::Supplement345:
      // Part Flags
      encodeV3Data = true;
      baseSize = LoadHeaderSizeV3;
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc665Exception{} << ArincSupport::AdditionalInfo{ "Unsupported ARINC 665 Version" } );
  }

  ArincSupport::RawData rawFile( baseSize );

  // Part Flags or Spare
  ArincSupport::RawData_setInt< uint16_t >(
    ArincSupport::RawDataSpan{ rawFile }.subspan( PartFlagsFieldOffsetV3 ),
    encodeV3Data ? partFlagsV : 0U );

  // Next free Offset (used for optional pointer calculation)
  ptrdiff_t nextFreeOffset{ static_cast< ptrdiff_t>( rawFile.size() ) };

  // Load Part Number
  auto rawLoadPn{ StringUtils_encodeString( partNumber() ) };
  assert( rawLoadPn.size() % 2 == 0 );

  ArincSupport::RawData_setInt< uint32_t >(
    ArincSupport::RawDataSpan{ rawFile }.subspan( LoadPartNumberPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2 ) );
  nextFreeOffset += static_cast< ptrdiff_t>( rawLoadPn.size() );

  rawFile.insert( rawFile.end(), rawLoadPn.begin(), rawLoadPn.end() );


  // Load Type (only in V3 mode)
  if ( encodeV3Data )
  {
    uint32_t loadTypePtr{ 0 };

    // Encode lode type only if set.
    if ( typeV )
    {
      loadTypePtr = static_cast< uint32_t >( nextFreeOffset / 2 );

      const auto rawTypeDescription{ StringUtils_encodeString( typeV->first ) };
      assert( rawTypeDescription.size() % 2 == 0 );

      // description
      rawFile.insert( rawFile.end(), rawTypeDescription.begin(), rawTypeDescription.end() );

      rawFile.resize( rawFile.size() + sizeof( uint16_t ) );
      ArincSupport::RawData_setInt< uint16_t >(
        ArincSupport::RawDataSpan{ rawFile }.subspan( nextFreeOffset + static_cast< ptrdiff_t >( rawTypeDescription.size() ) ),
        typeV->second );

      nextFreeOffset += static_cast< ptrdiff_t>( rawTypeDescription.size() + sizeof( uint16_t ) );
    }

    ArincSupport::RawData_setInt< uint32_t >(
      ArincSupport::RawDataSpan{ rawFile }.subspan( LoadTypeDescriptionPointerFieldOffsetV3 ),
      loadTypePtr );
  }

  // THW ID list
  auto rawThwIdsList{ StringUtils_encodeStrings( targetHardwareIdsV ) };
  assert( rawThwIdsList.size() % 2 == 0 );

  ArincSupport::RawData_setInt< uint32_t >(
    ArincSupport::RawDataSpan{ rawFile }.subspan( ThwIdsPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2 ) );
  nextFreeOffset += static_cast< ptrdiff_t >( rawThwIdsList.size() );

  rawFile.insert( rawFile.end(), rawThwIdsList.begin(), rawThwIdsList.end() );


  // THW ID + Positions (only in V3 mode)
  if ( encodeV3Data )
  {
    uint16_t thwIdPosCount{ 0 };
    ArincSupport::RawData rawThwPos( sizeof( uint16_t ) );

    for ( const auto &[ thwId, positions ] : targetHardwareIdsPositionsV )
    {
      // skip if no positions are stored
      if ( positions.empty() )
      {
        continue;
      }

      // THW ID
      const auto rawThwId{ StringUtils_encodeString( thwId ) };
      assert( rawThwId.size() % 2 == 0 );

      rawThwPos.insert( rawThwPos.end(), rawThwId.begin(), rawThwId.end() );

      // Positions
      const auto rawPositions{ StringUtils_encodeStrings( positions ) };
      assert( rawThwId.size() % 2 == 0 );

      rawThwPos.insert( rawThwPos.end(), rawPositions.begin(), rawPositions.end() );

      ++thwIdPosCount;
    }

    ArincSupport::RawData_setInt< uint16_t>( rawThwPos, thwIdPosCount );

    uint32_t thwIdPosPtr{ 0 };

    if ( 0 != thwIdPosCount )
    {
      thwIdPosPtr = static_cast< uint32_t >( nextFreeOffset / 2 );
      rawFile.insert( rawFile.end(), rawThwPos.begin(), rawThwPos.end() );
      nextFreeOffset += static_cast< ptrdiff_t>( rawThwPos.size() );
    }

    ArincSupport::RawData_setInt< uint32_t >(
      ArincSupport::RawDataSpan{ rawFile }.subspan( ThwIdPositionsPointerFieldOffsetV3 ),
      thwIdPosPtr );
  }


  // data files list pointer
  auto rawDataFiles{ encodeDataFiles( encodeV3Data ) };
  assert( rawDataFiles.size() % 2 == 0 );

  ArincSupport::RawData_setInt< uint32_t>(
    ArincSupport::RawDataSpan{ rawFile }.subspan( DataFilesPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2 ) );
  nextFreeOffset += static_cast< ptrdiff_t>( rawDataFiles.size() );

  rawFile.insert( rawFile.end(), rawDataFiles.begin(), rawDataFiles.end() );


  // support files (only if support files are present)
  auto rawSupportFiles{ encodeSupportFiles( encodeV3Data ) };
  assert( rawSupportFiles.size() % 2 == 0 );

  uint32_t supportFileListPtr{ 0 };

  if ( !supportFiles().empty() )
  {
    supportFileListPtr = static_cast< uint32_t >( nextFreeOffset / 2 );
    nextFreeOffset += static_cast< ptrdiff_t>( rawSupportFiles.size() );

    rawFile.insert( rawFile.end(), rawSupportFiles.begin(), rawSupportFiles.end() );
  }

  ArincSupport::RawData_setInt< uint32_t >(
    ArincSupport::RawDataSpan{ rawFile }.subspan( SupportFilesPointerFieldOffsetV2 ),
    supportFileListPtr );

  // user defined data pointer
  assert( userDefinedDataV.size() % 2 == 0 );
  uint32_t userDefinedDataPtr{ 0 };

  if ( !userDefinedDataV.empty() )
  {
    userDefinedDataPtr = static_cast< uint32_t >( nextFreeOffset / 2 );
    nextFreeOffset += static_cast< ptrdiff_t>( userDefinedDataV.size() );

    rawFile.insert( rawFile.end(), userDefinedDataV.begin(), userDefinedDataV.end() );
  }

  ArincSupport::RawData_setInt< uint32_t >(
    ArincSupport::RawDataSpan{ rawFile }.subspan( UserDefinedDataPointerFieldOffsetV2 ),
    userDefinedDataPtr );

  // amount of data of Check Values and CRCs
  // by default File CRC (16bit) + load CRC (32 bit)
  uint32_t checkValueCrcSizes{ sizeof( uint16_t ) + sizeof( uint32_t ) };

  // Load Check Value (only in V3 mode)
  if ( encodeV3Data )
  {
    // Alternative implementation set Load Check Pointer to zero, when Load
    // Check Value is not given

    checkValueCrcSizes += static_cast< uint32_t >( CheckValueUtils_size( loadCheckValueTypeV ) );

    // Set Pointer to Load Check Value Field
    ArincSupport::RawData_setInt< uint32_t >(
      ArincSupport::RawDataSpan{ rawFile }.subspan( LoadCheckValuePointerFieldOffsetV3 ),
      static_cast< uint32_t >( nextFreeOffset / 2 ) );

    // actual check value must be encoded by external means
  }

  // set header
  insertHeader( rawFile, checkValueCrcSizes );

  // Resize to final size ( Check Value + File CRC + Load CRC)
  rawFile.resize( rawFile.size() + checkValueCrcSizes );

  // set CRC
  calculateFileCrc( rawFile );

  // load CRC must be encoded by external means

  return rawFile;
}

void LoadHeaderFile::decodeBody( ArincSupport::ConstRawDataSpan rawFile )
{
  bool decodeV3Data{ false };

  uint16_t partFlags;
  std::tie( std::ignore, partFlags ) = ArincSupport::RawData_getInt< uint16_t >( rawFile.subspan( PartFlagsFieldOffsetV3 ) );

  switch ( arincVersion() )
  {
    case SupportedArinc665Version::Supplement2:
      // Spare
      if ( partFlags != 0U )
      {
        partFlagsV = 0;
        BOOST_THROW_EXCEPTION( Arinc665Exception{} << ArincSupport::AdditionalInfo{ "Spare not 0" } );
      }
      break;

    case SupportedArinc665Version::Supplement345:
      partFlagsV = partFlags;
      decodeV3Data = true;
      break;

    default:
      BOOST_THROW_EXCEPTION( Arinc665Exception{} << ArincSupport::AdditionalInfo{ "Unsupported ARINC 665 Version" } );
  }

  uint32_t loadPartNumberPtr{};
  std::tie( std::ignore, loadPartNumberPtr ) =
    ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( LoadPartNumberPointerFieldOffsetV2 ) );

  uint32_t targetHardwareIdListPtr{};
  std::tie( std::ignore, targetHardwareIdListPtr ) =
    ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( ThwIdsPointerFieldOffsetV2 ) );

  uint32_t dataFileListPtr{};
  std::tie( std::ignore, dataFileListPtr ) =
    ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( DataFilesPointerFieldOffsetV2 ) );

  uint32_t supportFileListPtr{};
  std::tie( std::ignore, supportFileListPtr ) =
    ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( SupportFilesPointerFieldOffsetV2 ) );

  uint32_t userDefinedDataPtr{};
  std::tie( std::ignore, userDefinedDataPtr ) =
    ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( UserDefinedDataPointerFieldOffsetV2 ) );

  uint32_t loadTypeDescriptionPtr{ 0U };
  uint32_t thwIdsPositionPtr{ 0U };
  uint32_t loadCheckValuePtr{ 0U };

  // only decode this pointers in V3 mode
  if ( decodeV3Data )
  {
    std::tie( std::ignore, loadTypeDescriptionPtr ) =
      ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( LoadTypeDescriptionPointerFieldOffsetV3 ) );

    std::tie( std::ignore, thwIdsPositionPtr ) =
      ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( ThwIdPositionsPointerFieldOffsetV3 ) );

    std::tie( std::ignore, loadCheckValuePtr ) =
      ArincSupport::RawData_getInt< uint32_t >( rawFile.subspan( LoadCheckValuePointerFieldOffsetV3 ) );
  }


  // load part number
  std::tie( std::ignore, partNumberV ) = StringUtils_decodeString( rawFile.subspan( loadPartNumberPtr * 2ULL ) );


  // Load Type Description Field (ARINC 665-3)
  typeV.reset();
  if ( decodeV3Data && ( 0!=loadTypeDescriptionPtr ) )
  {
    auto [ remaining, loadTypeDescription ]{
      StringUtils_decodeString( rawFile.subspan( loadTypeDescriptionPtr * 2ULL ) ) };

    auto [ _, loadTypeValue ]{ ArincSupport::RawData_getInt< uint16_t>( remaining ) };

    loadType( LoadType{ { std::string{ loadTypeDescription }, loadTypeValue } } );
  }

  // Target Hardware ID list
  TargetHardwareIds targetHardwareIdsValue;
  std::tie( std::ignore, targetHardwareIdsValue ) =
    StringUtils_decodeStrings( rawFile.subspan( targetHardwareIdListPtr * 2ULL ) );
  targetHardwareIds( std::move( targetHardwareIdsValue ) );

  // THW IDs with Positions Field (ARINC 665-3)
  targetHardwareIdsPositionsV.clear();
  if ( decodeV3Data && ( 0U != thwIdsPositionPtr ) )
  {
    auto [ remaining, numberOfThwIdsWithPos ]{
      ArincSupport::RawData_getInt< uint16_t >( rawFile.subspan( thwIdsPositionPtr * 2ULL ) ) };

    for ( uint16_t thwIdIndex{ 0 }; thwIdIndex < numberOfThwIdsWithPos; ++thwIdIndex )
    {
      std::string thwId;
      std::tie( remaining, thwId ) = StringUtils_decodeString( remaining );

      Positions positions;
      std::tie( remaining, positions ) = StringUtils_decodeStrings( remaining );

      targetHardwareIdPositions( std::move( thwId ), std::move( positions ) );
    }
  }

  // data file list
  decodeDataFiles( rawFile.subspan( dataFileListPtr * 2ULL ), decodeV3Data );

  // support file list
  if ( 0U != supportFileListPtr )
  {
    decodeSupportFiles( rawFile.subspan( supportFileListPtr * 2ULL ), decodeV3Data );
  }

  // user defined data
  userDefinedDataV.clear();
  if ( 0U != userDefinedDataPtr )
  {
    ptrdiff_t endOfUserDefinedData{ static_cast<ptrdiff_t>(rawFile.size()) - static_cast<ptrdiff_t>(FileCrcOffset) };

    if ( loadCheckValuePtr != 0 )
    {
      if ( loadCheckValuePtr <= userDefinedDataPtr )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << ArincSupport::AdditionalInfo{ "Invalid Pointers" } );
      }

      endOfUserDefinedData = loadCheckValuePtr * 2LL;
    }

    userDefinedDataV.assign( rawFile.begin() + userDefinedDataPtr * 2LL, rawFile.begin() + endOfUserDefinedData );
  }

  // Load Check Value Field (ARINC 665-3) - Only Type is stored.
  // Check must be performed by other means
  loadCheckValueTypeV = ArincChecksum::CheckValueType::NotUsed;
  if ( decodeV3Data && ( 0U!=loadCheckValuePtr ) )
  {
    loadCheckValueTypeV = CheckValueUtils_decode( rawFile.subspan( loadCheckValuePtr * 2ULL ) ).type();
  }

  // file crc decoded and checked within base class

  // load crc is not decoded - this must be done by other means
}

ArincSupport::RawData LoadHeaderFile::encodeDataFiles( const bool encodeV3Data ) const
{
  ArincSupport::RawData rawFileList( sizeof( uint16_t ) );

  // Number of files must not exceed field
  if ( dataFilesV.size() > std::numeric_limits< uint16_t >::max() )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File{} << ArincSupport::AdditionalInfo{ "More files than allowed" } );
  }

  // number of files
  ArincSupport::RawData_setInt< uint16_t >( rawFileList, ArincSupport::safeCast< uint16_t >( dataFilesV.size() ) );

  // iterate over files
  uint16_t fileCounter{ 0 };
  for ( auto const &fileInfo : dataFilesV )
  {
    ++fileCounter;

    ArincSupport::RawData rawFileInfo( sizeof( uint16_t ) );

    // filename
    auto const rawFilename{ StringUtils_encodeString( fileInfo.filename ) };
    assert( rawFilename.size() % 2 == 0);
    rawFileInfo.insert( rawFileInfo.end(), rawFilename.begin(), rawFilename.end() );

    // part number
    auto const rawPartNumber{ StringUtils_encodeString( fileInfo.partNumber ) };
    assert( rawPartNumber.size() % 2 == 0 );
    rawFileInfo.insert( rawFileInfo.end(), rawPartNumber.begin(), rawPartNumber.end() );

    // resize for file length, CRC
    rawFileInfo.resize( rawFileInfo.size() + sizeof( uint32_t ) + sizeof( uint16_t ) );

    // file length - rounded number of 16-bit words
    const uint32_t fileLength{ ArincSupport::safeCast< uint32_t >( ( fileInfo.length + 1U ) / 2U ) };

    ArincSupport::RawData_setInt< uint32_t >(
      ArincSupport::RawDataSpan{ rawFileInfo }.last( sizeof( uint32_t ) + sizeof( uint16_t ) ),
      fileLength );

    // CRC
    ArincSupport::RawData_setInt< uint16_t >( ArincSupport::RawDataSpan{ rawFileInfo }.last( sizeof( uint16_t ) ), fileInfo.crc );

    // following fields are available in ARINC 665-3 ff
    if ( encodeV3Data )
    {
      // length in bytes (Data File List)
      rawFileInfo.resize( rawFileInfo.size() + sizeof( uint64_t ) );
      ArincSupport::RawData_setInt< uint64_t >(
        ArincSupport::RawDataSpan{ rawFileInfo }.last( sizeof( uint64_t ) ),
        fileInfo.length );

      // check Value
      const auto rawCheckValue{ CheckValueUtils_encode( fileInfo.checkValue ) };
      assert( rawCheckValue.size() % 2 == 0 );
      rawFileInfo.insert( rawFileInfo.end(), rawCheckValue.begin(), rawCheckValue.end() );
    }

    // next load pointer (is set to 0 for last load)
    ArincSupport::RawData_setInt< uint16_t >(
      rawFileInfo,
      ( fileCounter == dataFilesV.size() ) ? 0U : ArincSupport::safeCast< uint16_t >( rawFileInfo.size() / 2U ) );

    // add file info to files info
    rawFileList.insert( rawFileList.end(), rawFileInfo.begin(), rawFileInfo.end() );
  }

  return rawFileList;
}

ArincSupport::RawData LoadHeaderFile::encodeSupportFiles( const bool encodeV3Data ) const
{
  ArincSupport::RawData rawFileList( sizeof( uint16_t ) );

  // Number of files must not exceed field
  if ( supportFilesV.size() > std::numeric_limits< uint16_t>::max() )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File{} << ArincSupport::AdditionalInfo{ "More files than allowed" } );
  }

  // number of loads
  ArincSupport::RawData_setInt< uint16_t >( rawFileList, ArincSupport::safeCast< uint16_t >( supportFilesV.size() ) );

  // iterate over files
  uint16_t fileCounter{ 0 };
  for ( auto const &fileInfo : supportFilesV )
  {
    ++fileCounter;

    ArincSupport::RawData rawFileInfo( sizeof( uint16_t ) );

    // filename
    auto const rawFilename{ StringUtils_encodeString( fileInfo.filename ) };
    assert( rawFilename.size() % 2 == 0 );
    rawFileInfo.insert( rawFileInfo.end(), rawFilename.begin(), rawFilename.end() );

    // part number
    auto const rawPartNumber{ StringUtils_encodeString( fileInfo.partNumber ) };
    assert( rawPartNumber.size() % 2 == 0);
    rawFileInfo.insert( rawFileInfo.end(), rawPartNumber.begin(), rawPartNumber.end() );

    // resize for file length and CRC
    rawFileInfo.resize( rawFileInfo.size() + sizeof( uint32_t ) + sizeof( uint16_t ) );

    // file length in number of bytes
    uint32_t fileLength{ ArincSupport::safeCast< uint32_t>( fileInfo.length ) };

    ArincSupport::RawData_setInt< uint32_t >(
      ArincSupport::RawDataSpan{ rawFileInfo }.last( sizeof( uint32_t ) + sizeof( uint16_t ) ),
      fileLength );

    // CRC
    ArincSupport::RawData_setInt< uint16_t >( ArincSupport::RawDataSpan{ rawFileInfo }.last( sizeof( uint16_t ) ), fileInfo.crc );

    // following fields are available in ARINC 665-3 ff
    if ( encodeV3Data )
    {
      // check Value
      const auto rawCheckValue{ CheckValueUtils_encode( fileInfo.checkValue ) };
      assert( rawCheckValue.size() % 2 == 0 );
      rawFileInfo.insert( rawFileInfo.end(), rawCheckValue.begin(), rawCheckValue.end() );
    }

    // next load pointer (is set to 0 for last load)
    ArincSupport::RawData_setInt< uint16_t >(
      rawFileInfo,
      ( fileCounter == supportFilesV.size() ) ? 0U : ArincSupport::safeCast< uint16_t >( rawFileInfo.size() / 2U ) );

    // add file info to files info
    rawFileList.insert( rawFileList.end(), rawFileInfo.begin(), rawFileInfo.end() );
  }

  return rawFileList;
}

void LoadHeaderFile::decodeDataFiles( ArincSupport::ConstRawDataSpan rawData, const bool decodeV3Data )
{
  auto remaining{ rawData };

  // clear data files
  dataFilesV.clear();

  // number of data files
  uint16_t numberOfFiles;
  std::tie( remaining, numberOfFiles ) = ArincSupport::RawData_getInt< uint16_t>( remaining );

  // iterate over file index
  for ( uint16_t fileIndex = 0; fileIndex < numberOfFiles; ++fileIndex )
  {
    auto listRemaining{ remaining };

    // next file pointer
    uint16_t filePointer;
    std::tie( listRemaining, filePointer ) = ArincSupport::RawData_getInt< uint16_t>( listRemaining );

    // check file pointer for validity
    if ( fileIndex != numberOfFiles - 1U )
    {
      if ( filePointer == 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << ArincSupport::AdditionalInfo{ "next file pointer is 0" } );
      }
    }
    else
    {
      if ( filePointer != 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << ArincSupport::AdditionalInfo{ "next file pointer is not 0" } );
      }
    }

    // filename
    std::string name;
    std::tie( listRemaining, name ) = StringUtils_decodeString( listRemaining );

    // part number
    std::string partNumber;
    std::tie( listRemaining, partNumber ) = StringUtils_decodeString( listRemaining );

    // file length
    uint32_t length;
    std::tie( listRemaining, length ) = ArincSupport::RawData_getInt< uint32_t>( listRemaining );

    // rounded number of 16-bit words
    uint64_t realLength{ length * 2ULL };

    // CRC
    uint16_t crc;
    std::tie( listRemaining, crc ) = ArincSupport::RawData_getInt< uint16_t >( listRemaining );

    // CheckValue - if not decoded "no check value"
    ArincChecksum::CheckValue checkValue{ ArincChecksum::CheckValue::NoCheckValue };

    // following fields are available in ARINC 665-3 ff
    if ( decodeV3Data )
    {
      // length in bytes (Data File List)
      uint64_t fileLengthInBytes{};
      std::tie( listRemaining, fileLengthInBytes ) = ArincSupport::RawData_getInt< uint64_t>( listRemaining );

      // check length fields for consistency
      if ( ( ( fileLengthInBytes + 1 ) / 2 ) <= std::numeric_limits< uint32_t >::max()
        && ( length != ( static_cast< uint32_t >( (fileLengthInBytes + 1) / 2 ) ) ) )
      {
        BOOST_THROW_EXCEPTION( Arinc665Exception()
          << ArincSupport::AdditionalInfo{ "Inconsistent length fields" } );
      }

      // update real length
      realLength = fileLengthInBytes;

      // check Value
      checkValue = CheckValueUtils_decode( listRemaining );
    }

    // set it to begin of next file
    remaining = remaining.subspan( filePointer * 2ULL );

    // file info
    dataFilesV.emplace_back( LoadFileInfo{
      .filename = std::move( name ),
      .partNumber = std::move( partNumber ),
      .length = realLength,
      .crc = crc,
      .checkValue = std::move( checkValue ) } );
  }
}

void LoadHeaderFile::decodeSupportFiles( ArincSupport::ConstRawDataSpan rawData, bool decodeV3Data )
{
  auto remaining{ rawData };

  // clear support files
  supportFilesV.clear();

  // number of data files
  uint16_t numberOfFiles{};
  std::tie( remaining, numberOfFiles ) = ArincSupport::RawData_getInt< uint16_t>( remaining );

  // iterate over file index
  for ( uint16_t fileIndex = 0; fileIndex < numberOfFiles; ++fileIndex )
  {
    auto listRemaining{ remaining };

    // next file pointer
    uint16_t filePointer{};
    std::tie( listRemaining, filePointer ) = ArincSupport::RawData_getInt< uint16_t >( listRemaining );

    // check file pointer for validity
    if ( fileIndex != numberOfFiles - 1U )
    {
      if ( filePointer == 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << ArincSupport::AdditionalInfo{ "next file pointer is 0" } );
      }
    }
    else
    {
      if ( filePointer != 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << ArincSupport::AdditionalInfo{ "next file pointer is not 0" } );
      }
    }

    // filename
    std::string name{};
    std::tie( listRemaining, name ) = StringUtils_decodeString( listRemaining );

    // part number
    std::string partNumber{};
    std::tie( listRemaining, partNumber ) = StringUtils_decodeString( listRemaining );

    // file length
    uint32_t length{};
    std::tie( listRemaining, length ) = ArincSupport::RawData_getInt< uint32_t >( listRemaining );

    // CRC
    uint16_t crc{};
    std::tie( listRemaining, crc ) = ArincSupport::RawData_getInt< uint16_t >( listRemaining );

    // CheckValue - if not decoded "no check value"
    ArincChecksum::CheckValue checkValue{ ArincChecksum::CheckValue::NoCheckValue };

    // following fields are available in ARINC 665-3 ff
    if ( decodeV3Data )
    {
      // check Value
      checkValue = CheckValueUtils_decode( listRemaining );
    }

    // set it to begin of next file
    remaining = remaining.subspan( filePointer * 2ULL );

    // file info
    supportFilesV.emplace_back(
      LoadFileInfo{ std::move( name ), std::move( partNumber ), length, crc, std::move( checkValue ) } );
  }
}

void LoadHeaderFile::checkUserDefinedData()
{
  if ( userDefinedDataV.size() % 2U != 0U )
  {
    ARINC_LOG_WARN( "User defined data must be 2-byte aligned. - extending range" );

    userDefinedDataV.push_back( std::byte{ 0U } );
  }
}

}
