// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::LoadListFile.
 **/

#include "LoadListFile.hpp"

#include <arinc_665/files/StringUtils.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>
#include <helper/RawData.hpp>
#include <helper/SafeCast.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

LoadListFile::LoadListFile( const SupportedArinc665Version version ) :
  ListFile{ version }
{
}

LoadListFile::LoadListFile( Helper::ConstRawDataSpan rawFile ) :
  ListFile{ rawFile, FileType::LoadList }
{
  decodeBody( rawFile );
}

LoadListFile& LoadListFile::operator=( Helper::ConstRawDataSpan rawFile )
{
  // call inherited operator
  Arinc665File::operator =( rawFile );
  decodeBody( rawFile );

  return *this;
}

FileType LoadListFile::fileType() const noexcept
{
  return FileType::LoadList;
}

size_t LoadListFile::numberOfLoads() const
{
  return loadsV.size();
}

const LoadsInfo& LoadListFile::loads() const
{
  return loadsV;
}

LoadsInfo& LoadListFile::loads()
{
  return loadsV;
}

void LoadListFile::loads( LoadsInfo loadsInformation )
{
  loadsV = std::move( loadsInformation );
}

void LoadListFile::load( LoadInfo load )
{
  loadsV.push_back( std::move( load ) );
}

Helper::ConstRawDataSpan LoadListFile::userDefinedData() const
{
  return userDefinedDataV;
}

void LoadListFile::userDefinedData( Helper::RawData userDefinedData )
{
  userDefinedDataV = std::move( userDefinedData);

  checkUserDefinedData();
}

bool LoadListFile::belongsToSameMediaSet( const LoadListFile &other ) const
{
  return ( mediaSetPn() == other.mediaSetPn() )
    && ( numberOfMediaSetMembers() == other.numberOfMediaSetMembers() )
    && std::ranges::equal( userDefinedDataV, other.userDefinedData() )
    && ( loadsV == other.loads() );
}

Helper::RawData LoadListFile::encode() const
{
  Helper::RawData rawFile( FileHeaderSizeV2 );

  // Spare Field
  Helper::RawData_setInt< uint16_t>( Helper::RawDataSpan{ rawFile }.subspan( SpareFieldOffsetV2 ), 0U );

  // Next free Offset (used for optional pointer calculation)
  ptrdiff_t nextFreeOffset{ static_cast< ptrdiff_t >( rawFile.size() ) };

  // media set information
  const auto rawMediaInformation{ encodeMediaInformation() };
  assert( rawMediaInformation.size() % 2 == 0);
  rawFile.insert( rawFile.end(), rawMediaInformation.begin(), rawMediaInformation.end() );

  // media set information pointer
  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan( MediaSetPartNumberPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2U ) );
  nextFreeOffset += static_cast< ptrdiff_t >( rawMediaInformation.size() );

  // loads info
  const auto rawLoadsInfo{ encodeLoadsInfo() };
  assert( rawLoadsInfo.size() % 2 == 0);

  // loads list pointer
  Helper::RawData_setInt< uint32_t >(
    Helper::RawDataSpan{ rawFile }.subspan( LoadFilesPointerFieldOffsetV2 ),
    static_cast< uint32_t >( nextFreeOffset / 2U ) );
  nextFreeOffset += static_cast< ptrdiff_t >( rawLoadsInfo.size() );

  rawFile.insert( rawFile.end(), rawLoadsInfo.begin(), rawLoadsInfo.end() );

  // user defined data
  assert( userDefinedDataV.size() % 2 == 0 );
  uint32_t userDefinedDataPtr{};

  if ( !userDefinedDataV.empty() )
  {
    userDefinedDataPtr = static_cast< uint32_t >( nextFreeOffset / 2U );

    rawFile.insert( rawFile.end(), userDefinedDataV.begin(), userDefinedDataV.end() );
  }

  Helper::RawData_setInt< uint32_t>(
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

void LoadListFile::decodeBody( Helper::ConstRawDataSpan rawFile )
{
  // Spare Field
  auto [ _, spare ]{ Helper::RawData_getInt< uint16_t >( rawFile.subspan( SpareFieldOffsetV2 ) ) };
  if ( 0U != spare )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "Spare is not 0" } );
  }

  // media information pointer
  auto [ _1, mediaInformationPtr ]{
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( MediaSetPartNumberPointerFieldOffsetV2 ) ) };

  // Loads list pointer
  auto [ _2, loadListPtr ]{
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( LoadFilesPointerFieldOffsetV2 ) ) };

  // user defined data pointer
  auto [ _3, userDefinedDataPtr ]{
    Helper::RawData_getInt< uint32_t >( rawFile.subspan( UserDefinedDataPointerFieldOffsetV2 ) ) };

  // decode Media Information
  decodeMediaInformation( rawFile.subspan( 2ULL * mediaInformationPtr ) );

  // Loads list
  decodeLoadsInfo( rawFile.subspan( loadListPtr * 2ULL ) );

  // user defined data
  if ( 0 != userDefinedDataPtr )
  {
    userDefinedDataV.assign(
      rawFile.begin() + static_cast< ptrdiff_t >( userDefinedDataPtr ) * 2,
      rawFile.begin() + static_cast< ptrdiff_t >( rawFile.size() ) - DefaultChecksumPosition );
  }

  // file crc decoded and checked within base class
}

Helper::RawData LoadListFile::encodeLoadsInfo() const
{
  Helper::RawData rawLoadsInfo( sizeof( uint16_t ) );

  // Number of loads must not exceed field
  if ( loadsV.size() > std::numeric_limits< uint16_t>::max() )
  {
    BOOST_THROW_EXCEPTION( InvalidArinc665File()
      << Helper::AdditionalInfo{ "More loads than allowed" } );
  }

  // number of loads
  Helper::RawData_setInt< uint16_t >( rawLoadsInfo, Helper::safeCast< uint16_t >( loadsV.size() ) );

  // iterate over files
  uint16_t loadCounter{ 0 };
  for ( auto const &loadInfo : loadsV )
  {
    ++loadCounter;

    Helper::RawData rawLoadInfo( sizeof( uint16_t ) );

    // part number
    auto const rawPartNumber( StringUtils_encodeString( loadInfo.partNumber ) );
    assert( rawPartNumber.size() % 2 == 0);
    rawLoadInfo.insert( rawLoadInfo.end(), rawPartNumber.begin(), rawPartNumber.end() );

    // header filename
    auto const rawHeaderFilename( StringUtils_encodeString( loadInfo.headerFilename ) );
    assert( rawHeaderFilename.size() % 2 == 0);
    rawLoadInfo.insert( rawLoadInfo.end(), rawHeaderFilename.begin(), rawHeaderFilename.end() );

    // member sequence number
    rawLoadInfo.resize( rawLoadInfo.size() + sizeof( uint16_t ) );
    Helper::RawData_setInt< uint16_t >(
      Helper::RawDataSpan{ rawLoadInfo }.last( sizeof( uint16_t ) ),
      static_cast< uint8_t >( loadInfo.memberSequenceNumber ) );

    // THW IDs list
    auto const rawThwIds( StringUtils_encodeStrings( loadInfo.targetHardwareIds ) );
    assert( rawThwIds.size() % 2 == 0);
    rawLoadInfo.insert( rawLoadInfo.end(), rawThwIds.begin(), rawThwIds.end() );

    // next load pointer (is set to 0 for last load)
    Helper::RawData_setInt< uint16_t >(
      rawLoadInfo,
      ( loadCounter == numberOfLoads() ) ? 0U : static_cast< uint16_t >( rawLoadInfo.size() / 2U ) );

    // add file info to files info
    rawLoadsInfo.insert( rawLoadsInfo.end(), rawLoadInfo.begin(), rawLoadInfo.end() );
  }

  return rawLoadsInfo;
}

void LoadListFile::decodeLoadsInfo( Helper::ConstRawDataSpan rawData )
{
  auto remaining{ rawData };

  // number of loads
  uint16_t numberOfLoads{};
  std::tie( remaining, numberOfLoads ) = Helper::RawData_getInt< uint16_t>( remaining );

  // iterate of load counter
  for ( uint16_t loadIndex = 0; loadIndex < numberOfLoads; ++loadIndex )
  {
    auto listRemaining{ remaining };

    // next load pointer
    uint16_t loadPointer{};
    std::tie( listRemaining, loadPointer ) = Helper::RawData_getInt< uint16_t >( listRemaining );

    // check load pointer for validity
    if ( loadIndex != numberOfLoads - 1U )
    {
      if ( loadPointer == 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "next load pointer is 0" } );
      }
    }
    else
    {
      if ( loadPointer != 0U )
      {
        BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "next load pointer is not 0" } );
      }
    }

    // part number
    std::string partNumber;
    std::tie( listRemaining, partNumber ) = StringUtils_decodeString( listRemaining );

    // header filename
    std::string headerFilename;
    std::tie( listRemaining, headerFilename ) = StringUtils_decodeString( listRemaining );

    // member sequence number
    uint16_t fileMemberSequenceNumber;
    std::tie( listRemaining, fileMemberSequenceNumber ) = Helper::RawData_getInt< uint16_t >( listRemaining );
    if ( ( fileMemberSequenceNumber < 1U ) || ( fileMemberSequenceNumber > 255U ) )
    {
      BOOST_THROW_EXCEPTION( InvalidArinc665File{} << Helper::AdditionalInfo{ "member sequence number out of range" } );
    }

    LoadInfo::ThwIds thwIds;
    std::tie( std::ignore, thwIds ) = StringUtils_decodeStrings( listRemaining );

    loadsV.emplace_back( LoadInfo{
      .partNumber = std::move( partNumber ),
      .headerFilename = std::move( headerFilename ),
      .memberSequenceNumber = MediumNumber{ Helper::safeCast< uint8_t >( fileMemberSequenceNumber ) },
      .targetHardwareIds = std::move( thwIds ) } );

    // set it to begin of next load
    remaining = remaining.subspan( loadPointer * 2ULL );
  }
}

void LoadListFile::checkUserDefinedData()
{
  if ( userDefinedDataV.size() % 2U != 0U )
  {
    SPDLOG_WARN( "User defined data must be 2-byte aligned. - extending range" );

    userDefinedDataV.push_back( std::byte{ 0U } );
  }
}

}
