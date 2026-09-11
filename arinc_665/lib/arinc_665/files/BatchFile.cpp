// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::BatchFile.
 **/

#include "BatchFile.hpp"

#include <arinc_665/files/StringUtils.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>
#include <helper/SafeCast.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

BatchFile::BatchFile( const SupportedArinc665Version version ) :
  Arinc665File{ version }
{
}

BatchFile::BatchFile( Helper::ConstRawDataSpan rawFile ) :
  Arinc665File{ rawFile, FileType::BatchFile }
{
  decodeBody( rawFile );
}

BatchFile& BatchFile::operator=( Helper::ConstRawDataSpan rawFile )
{
  // call inherited operator
  Arinc665File::operator =( rawFile );
  decodeBody( rawFile );

  return *this;
}

FileType BatchFile::fileType() const noexcept
{
  return FileType::BatchFile;
}

std::string_view BatchFile::partNumber() const
{
  return partNumberV;
}

void BatchFile::partNumber( std::string partNumber )
{
  partNumberV = std::move( partNumber );
}

std::string_view BatchFile::comment() const
{
  return commentV;
}

void BatchFile::comment( std::string comment )
{
  commentV = std::move( comment);
}

const BatchTargetsInfo& BatchFile::targetsHardware() const
{
  return targetsHardwareV;
}

BatchTargetsInfo& BatchFile::targetsHardware()
{
  return targetsHardwareV;
}

void BatchFile::targetHardware( BatchTargetInfo targetHardwareInfo )
{
  targetsHardwareV.push_back( std::move( targetHardwareInfo ) );
}

Helper::RawData BatchFile::encode() const
{
  Helper::RawData rawFile( BatchFileHeaderSizeV2 );

  // spare field
  Helper::RawData_setInt< uint16_t>( Helper::RawDataSpan{ rawFile }.subspan( SpareFieldOffsetV2 ), 0U );


  // Next free Offset (used for optional pointer calculation)
  size_t nextFreeOffset{ BatchFileHeaderSizeV2 };


  // batch part number + comment
  auto rawBatchPn{ StringUtils_encodeString( partNumberV ) };
  assert( rawBatchPn.size() % 2 == 0 );
  auto rawComment{ StringUtils_encodeString( commentV ) };
  assert( rawComment.size() % 2 == 0 );

  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan( BatchPartNumberPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2U ) );
  nextFreeOffset += rawBatchPn.size() + rawComment.size();

  rawFile.insert( rawFile.end(), rawBatchPn.begin(), rawBatchPn.end() );
  rawFile.insert( rawFile.end(), rawComment.begin(), rawComment.end() );


  // THW ID load list
  auto rawThwIdsList{ encodeBatchTargetsInfo() };
  assert( rawThwIdsList.size() % 2 == 0 );

  Helper::RawData_setInt< uint32_t>(
    Helper::RawDataSpan{ rawFile }.subspan( ThwIdsPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2U ) );

  rawFile.insert( rawFile.end(), rawThwIdsList.begin(), rawThwIdsList.end() );

  // set header
  insertHeader( rawFile );

  // Resize file for file CRC
  rawFile.resize( rawFile.size() + sizeof( uint16_t ) );

  // set CRC
  calculateFileCrc( rawFile );

  return rawFile;
}

void BatchFile::decodeBody( Helper::ConstRawDataSpan rawFile )
{
  // Spare field
  auto [ _, spare ]{ Helper::RawData_getInt< uint16_t>( rawFile.subspan( SpareFieldOffsetV2 ) ) };

  if ( 0U != spare )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File()
      << Helper::AdditionalInfo{ "Spare is not 0" } );
  }

  auto [ _1, batchPartNumberPtr]{
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( BatchPartNumberPointerFieldOffsetV2 ) ) };

  auto [_2, targetHardwareIdListPtr]{
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( ThwIdsPointerFieldOffsetV2 ) ) };

  auto remaining{ rawFile.subspan( batchPartNumberPtr * 2ULL ) };

  // batch part number
  std::tie( remaining, partNumberV ) = StringUtils_decodeString( remaining );

  // comment
  std::tie( remaining, commentV ) = StringUtils_decodeString( remaining );

  // target hardware ID load list
  decodeBatchTargetsInfo( rawFile.subspan( targetHardwareIdListPtr * 2ULL ) );
}

Helper::RawData BatchFile::encodeBatchTargetsInfo() const
{
  Helper::RawData rawBatchTargetsInfo( sizeof( uint16_t ) ); // Number of THW IDs

  // Number of targets must not exceed field
  if ( targetsHardwareV.size() > std::numeric_limits< uint16_t>::max() )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File()
      << Helper::AdditionalInfo{ "More THW IDs than allowed" } );
  }

  Helper::RawData_setInt< uint16_t>( rawBatchTargetsInfo, Helper::safeCast< uint16_t>( targetsHardwareV.size() ) );

  // iterate over target HWs
  uint16_t thwCounter{ 0 };
  for ( auto const &targetHardwareInfo : targetsHardwareV )
  {
    ++thwCounter;

    Helper::RawData rawLoadsInfo{};
    /* iterate over loads */
    for ( auto const &loadInfo : targetHardwareInfo.loads )
    {
      auto const rawHeaderFilename{ StringUtils_encodeString( loadInfo.headerFilename ) };
      assert( rawHeaderFilename.size() % 2 == 0 );

      auto const rawPartNumber{ StringUtils_encodeString( loadInfo.partNumber ) };
      assert( rawPartNumber.size() % 2 == 0 );

      rawLoadsInfo.insert( rawLoadsInfo.end(), rawHeaderFilename.begin(), rawHeaderFilename.end() );
      rawLoadsInfo.insert( rawLoadsInfo.end(), rawPartNumber.begin(), rawPartNumber.end() );
    }
    assert( rawLoadsInfo.size() % 2 == 0);

    Helper::RawData rawBatchTargetInfo( sizeof( uint16_t ) );

    auto const rawThwIdPosition{ StringUtils_encodeString( targetHardwareInfo.targetHardwareIdPosition ) };
    assert( rawThwIdPosition.size() % 2 == 0);

    // THW ID + Position
    rawBatchTargetInfo.insert( rawBatchTargetInfo.end(), rawThwIdPosition.begin(), rawThwIdPosition.end() );

    rawBatchTargetInfo.resize( rawBatchTargetInfo.size() + sizeof( uint16_t ) );

    // Number of Loads
    Helper::RawData_setInt< uint16_t>(
      Helper::RawDataSpan{ rawBatchTargetInfo }.last( sizeof( uint16_t ) ),
      Helper::safeCast< uint16_t>( targetHardwareInfo.loads.size() ) );

    // Loads list
    rawBatchTargetInfo.insert( rawBatchTargetInfo.end(), rawLoadsInfo.begin(), rawLoadsInfo.end() );

    // next load pointer (is set to 0 for last load)
    Helper::RawData_setInt< uint16_t>(
      rawBatchTargetInfo,
      ( thwCounter == targetsHardwareV.size() ) ?
        0U :
        Helper::safeCast< uint16_t>( rawBatchTargetInfo.size() / 2 ) );

    // add THW info to files info
    rawBatchTargetsInfo.insert( rawBatchTargetsInfo.end(), rawBatchTargetInfo.begin(), rawBatchTargetInfo.end() );
  }

  return rawBatchTargetsInfo;
}

void BatchFile::decodeBatchTargetsInfo( Helper::ConstRawDataSpan rawData )
{
  auto remaining{ rawData };

  // clear potentially data
  targetsHardwareV.clear();

  // number of target HW IDs
  uint16_t numberOfTargetHardwareIds{};
  std::tie( remaining, numberOfTargetHardwareIds ) = Helper::RawData_getInt< uint16_t >( remaining );

  // iterate over THW ID index
  for ( uint16_t thwIdIndex{ 0U }; thwIdIndex < numberOfTargetHardwareIds; ++thwIdIndex )
  {
    auto listRemaining{ remaining };

    // next THW ID pointer
    uint16_t thwIdPointer{};
    std::tie( listRemaining, thwIdPointer ) = Helper::RawData_getInt< uint16_t>( listRemaining );

    // check file pointer for validity
    if ( thwIdIndex != numberOfTargetHardwareIds - 1U )
    {
      if ( thwIdPointer == 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File()
          << Helper::AdditionalInfo{ "next THW ID pointer is 0" } );
      }
    }
    else
    {
      if ( thwIdPointer != 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File()
          << Helper::AdditionalInfo{ "next THW ID pointer is not 0" } );
      }
    }

    // THW ID
    std::string thwId{};
    std::tie( listRemaining, thwId ) = StringUtils_decodeString( listRemaining );

    // Loads list
    BatchLoadsInfo batchLoadsInfo{};

    // number of loads
    uint16_t numberOfLoads{};
    std::tie( listRemaining, numberOfLoads ) = Helper::RawData_getInt< uint16_t>( listRemaining );

    // iterate over load index
    for ( uint16_t loadIndex{ 0U }; loadIndex < numberOfLoads; ++loadIndex )
    {
      // header filename
      std::string filename;
      std::tie( listRemaining, filename ) = StringUtils_decodeString( listRemaining );

      // Load PN
      std::string partNumber;
      std::tie( listRemaining, partNumber ) = StringUtils_decodeString( listRemaining );

      // Batch Load info
      batchLoadsInfo.emplace_back( BatchLoadInfo{
        .headerFilename = std::move( filename ),
        .partNumber = std::move( partNumber ) } );
    }

    // set it to begin of next file
    remaining = remaining.subspan( thwIdPointer * 2ULL );

    // THW ID info
    targetsHardwareV.emplace_back( BatchTargetInfo{
      .targetHardwareIdPosition = std::move( thwId ),
      .loads = std::move( batchLoadsInfo ) } );
  }
}

}
