// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::BatchListFile.
 **/

#include "BatchListFile.hpp"

#include <arinc_665/files/StringUtils.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>
#include <helper/SafeCast.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

BatchListFile::BatchListFile( const SupportedArinc665Version version ) :
  ListFile{ version }
{
}

BatchListFile::BatchListFile( Helper::ConstRawDataSpan rawFile ) :
  ListFile{ rawFile, FileType::BatchList }
{
  decodeBody( rawFile );
}

BatchListFile &BatchListFile::operator=( Helper::ConstRawDataSpan rawFile )
{
  Arinc665File::operator =( rawFile );
  decodeBody( rawFile );

  return *this;
}

FileType BatchListFile::fileType() const noexcept
{
  return FileType::BatchList;
}

size_t BatchListFile::numberOfBatches() const
{
  return batchesV.size();
}

const BatchesInfo& BatchListFile::batches() const
{
  return batchesV;
}

BatchesInfo& BatchListFile::batches()
{
  return batchesV;
}

void BatchListFile:: batches( BatchesInfo batches )
{
  batchesV = std::move( batches );
}

void BatchListFile::batch( BatchInfo batch )
{
  batchesV.push_back( std::move( batch ) );
}

Helper::ConstRawDataSpan BatchListFile::userDefinedData() const
{
  return userDefinedDataV;
}

void BatchListFile::userDefinedData( Helper::RawData userDefinedData )
{
  userDefinedDataV = std::move( userDefinedData );

  checkUserDefinedData();
}

bool BatchListFile::belongsToSameMediaSet( const BatchListFile &other ) const
{
  return ( mediaSetPn() == other.mediaSetPn() )
    && ( numberOfMediaSetMembers() == other.numberOfMediaSetMembers() )
    && std::ranges::equal( userDefinedDataV, other.userDefinedData() )
    && ( batchesV == other.batches() );
}

Helper::RawData BatchListFile::encode() const
{
  Helper::RawData rawFile( FileHeaderSizeV2 );

  // Spare Field
  Helper::RawData_setInt< uint16_t>( Helper::RawDataSpan{ rawFile }.subspan( SpareFieldOffsetV2 ), 0U );

  // Next free Offset (used for optional pointer calculation)
  size_t nextFreeOffset{ rawFile.size() };


  // media set information
  const auto rawMediaInformation{ encodeMediaInformation() };
  assert( rawMediaInformation.size() % 2 == 0 );
  rawFile.insert( rawFile.end(), rawMediaInformation.begin(), rawMediaInformation.end() );

  // Update Pointer
  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan(  MediaSetPartNumberPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2U ) );
  nextFreeOffset += static_cast< ptrdiff_t >( rawMediaInformation.size() );


  // Batch Information
  const auto rawBatchesInfo{ encodeBatchesInfo() };
  assert( rawBatchesInfo.size() % 2 == 0 );

  // batches list pointer
  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan( BatchFilesPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2U ) );
  nextFreeOffset += rawBatchesInfo.size();

  rawFile.insert( rawFile.end(), rawBatchesInfo.begin(), rawBatchesInfo.end() );


  // user defined data
  assert( userDefinedDataV.size() % 2 == 0 );
  uint32_t userDefinedDataPtr = 0;

  if ( !userDefinedDataV.empty() )
  {
    userDefinedDataPtr = static_cast< uint32_t >( nextFreeOffset / 2U );

    rawFile.insert( rawFile.end(), userDefinedDataV.begin(), userDefinedDataV.end() );
  }

  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan( UserDefinedDataPointerFieldOffsetV2 ),
    userDefinedDataPtr );

  // set header
  insertHeader( rawFile );

  // Resize file for file CRC
  rawFile.resize( rawFile.size() + sizeof( uint16_t ) );

  // set CRC
  calculateFileCrc( rawFile );

  return rawFile;
}

void BatchListFile::decodeBody( Helper::ConstRawDataSpan rawFile )
{
  // Spare Field
  auto [ _, spare ]{ Helper::RawData_getInt< uint16_t >( rawFile.subspan( SpareFieldOffsetV2 ) ) };

  if ( 0U != spare )
  {
    BOOST_THROW_EXCEPTION(
      InvalidArinc665File() << Helper::AdditionalInfo{ "Spare is not 0" } );
  }


  // media information pointer
  auto [ _1, mediaInformationPtr ]{
    Helper::RawData_getInt< uint32_t>( rawFile.subspan( MediaSetPartNumberPointerFieldOffsetV2 ) ) };

  // batch list pointer
  auto [ _2, batchListPtr ]{ Helper::RawData_getInt< uint32_t >( rawFile.subspan( BatchFilesPointerFieldOffsetV2 ) ) };

  // user defined data pointer
  auto [ _3, userDefinedDataPtr ]{
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( UserDefinedDataPointerFieldOffsetV2 ) ) };

  // decode media information
  decodeMediaInformation( rawFile.subspan( 2ULL * mediaInformationPtr ) );

  // decode batch list
  decodeBatchesInfo( rawFile.subspan( 2ULL * batchListPtr ) );

  // user defined data
  if ( 0 != userDefinedDataPtr )
  {
    userDefinedDataV.assign(
      rawFile.begin() + static_cast< ptrdiff_t >( userDefinedDataPtr ) * 2,
      rawFile.begin() + static_cast< ptrdiff_t >( rawFile.size() - DefaultChecksumPosition ) );
  }

  // file crc decoded and checked within base class
}

Helper::RawData BatchListFile::encodeBatchesInfo() const
{
  Helper::RawData rawBatchesInfo( sizeof( uint16_t ) );

  // Number of batches must not exceed field
  if ( batchesV.size() > std::numeric_limits< uint16_t>::max() )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "More batches than allowed" } );
  }

  // number of batches
  Helper::RawData_setInt< uint16_t >( rawBatchesInfo, static_cast< uint16_t >( batchesV.size() ) );

  // iterate over batches
  uint16_t batchCounter{ 0 };
  for ( auto const &batchInfo : batchesV )
  {
    ++batchCounter;

    Helper::RawData rawBatchInfo( sizeof( uint16_t ) );

    auto const rawPartNumber{ StringUtils_encodeString( batchInfo.partNumber ) };
    assert( rawPartNumber.size() % 2 == 0 );

    auto const rawFilename{ StringUtils_encodeString( batchInfo.filename ) };
    assert( rawFilename.size() % 2 == 0 );

    // next pointer
    Helper::RawData_setInt< uint16_t >(
      rawBatchInfo,
      ( batchCounter == numberOfBatches() )
        ? 0U
        : static_cast< uint16_t >(
            ( sizeof( uint16_t ) + rawPartNumber.size() + rawFilename.size() + sizeof( uint16_t ) ) / 2U ) );

    // Part Number
    rawBatchInfo.insert( rawBatchInfo.end(), rawPartNumber.begin(), rawPartNumber.end() );

    // Batch Filename
    rawBatchInfo.insert( rawBatchInfo.end(), rawFilename.begin(), rawFilename.end() );

    // member sequence number
    rawBatchInfo.resize( rawBatchInfo.size() + sizeof( uint16_t ) );

    Helper::RawData_setInt< uint16_t >(
      Helper::RawDataSpan{ rawBatchInfo }.last( sizeof( uint16_t ) ),
      static_cast< uint8_t >( batchInfo.memberSequenceNumber ) );

    // add file info to files info
    rawBatchesInfo.insert( rawBatchesInfo.end(), rawBatchInfo.begin(), rawBatchInfo.end() );
  }

  return rawBatchesInfo;
}

void BatchListFile::decodeBatchesInfo( Helper::ConstRawDataSpan rawData )
{
  auto remaining{ rawData };

  // clear eventually stored infos
  batchesV.clear();

  // number of batches
  uint16_t numberOfBatches;
  std::tie( remaining, numberOfBatches ) = Helper::RawData_getInt< uint16_t >( remaining );

  // iterate over batch indexes
  for ( uint16_t batchIndex = 0; batchIndex < numberOfBatches; ++batchIndex )
  {
    auto listRemaining{ remaining };

    // next batch pointer
    uint16_t batchPointer{};
    std::tie( listRemaining, batchPointer ) = Helper::RawData_getInt< uint16_t>( listRemaining );

    // check batch pointer for validity
    if ( batchIndex != numberOfBatches - 1U )
    {
      if ( batchPointer == 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "next batch pointer is 0" } );
      }
    }
    else
    {
      if ( batchPointer != 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "next batch pointer is not 0" } );
      }
    }

    // part number
    std::string partNumber;
    std::tie( listRemaining, partNumber ) = StringUtils_decodeString( listRemaining );

    // batch filename
    std::string filename;
    std::tie( listRemaining, filename ) = StringUtils_decodeString( listRemaining );

    // member sequence number
    uint16_t memberSequenceNumber{};
    std::tie( listRemaining, memberSequenceNumber ) = Helper::RawData_getInt< uint16_t >( listRemaining );
    if ( ( memberSequenceNumber < 1U ) || ( memberSequenceNumber > 255U ) )
    {
      BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo( "member sequence number out of range" ) );
    }

    // set it to begin of next batch
    remaining = remaining.subspan( batchPointer * 2ULL );

    batchesV.emplace_back( BatchInfo{
      std::move( partNumber ),
      std::move( filename ),
      MediumNumber{ static_cast< uint8_t >( memberSequenceNumber ) } } );
  }
}

void BatchListFile::checkUserDefinedData()
{
  if ( userDefinedDataV.size() % 2U != 0U )
  {
    SPDLOG_WARN( "User defined data must be 2-byte aligned. - extending range" );

    userDefinedDataV.push_back( std::byte{ 0U } );
  }
}

}
