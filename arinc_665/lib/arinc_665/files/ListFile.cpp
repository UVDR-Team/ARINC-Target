// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Files::ListFile.
 **/

#include "ListFile.hpp"

#include <arinc_665/files/StringUtils.hpp>
#include <arinc_665/files/MediaSetInformation.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>
#include <helper/RawData.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Files {

std::string_view ListFile::mediaSetPn() const
{
  return mediaSetPnV;
}

void ListFile::mediaSetPn( std::string mediaSetPn )
{
  mediaSetPnV = std::move( mediaSetPn );
}

MediumNumber ListFile::mediaSequenceNumber() const
{
  return mediaSequenceNumberV;
}

void ListFile::mediaSequenceNumber( const MediumNumber mediaSequenceNumber )
{
  mediaSequenceNumberV = mediaSequenceNumber;
}

MediumNumber ListFile::numberOfMediaSetMembers() const
{
  return numberOfMediaSetMembersV;
}

void ListFile::numberOfMediaSetMembers( const MediumNumber numberOfMediaSetMembers )
{
  numberOfMediaSetMembersV = numberOfMediaSetMembers;
}

MediaSetInformation ListFile::mediaSetInformation() const
{
  return {
    .partNumber = mediaSetPnV,
    .mediaSequenceNumber = mediaSequenceNumberV,
    .numberOfMediaSetMembers = numberOfMediaSetMembersV };
}

ListFile::ListFile( SupportedArinc665Version version, ptrdiff_t checksumPosition ) noexcept :
  Arinc665File{ version, checksumPosition }
{
}

ListFile::ListFile(
  Helper::ConstRawDataSpan rawFile,
  const FileType expectedFileType,
  const ptrdiff_t checksumPosition ) :
  Arinc665File{ rawFile, expectedFileType, checksumPosition }
{
}

Helper::RawData ListFile::encodeMediaInformation() const
{
  // media set part number
  auto rawMediaInformation{ StringUtils_encodeString( mediaSetPn() ) };
  assert( rawMediaInformation.size() % 2 == 0U );

  const auto partNumberSize{ static_cast< ptrdiff_t >( rawMediaInformation.size() ) };

  rawMediaInformation.resize( partNumberSize + ( 2U * sizeof( uint8_t ) ) );
  auto remaining{ Helper::RawDataSpan{ rawMediaInformation}.subspan( partNumberSize ) };

  // media sequence number
  remaining = Helper::RawData_setInt( remaining, static_cast< uint8_t >( mediaSequenceNumberV ) );

  // number of media set members
  Helper::RawData_setInt< uint8_t>( remaining, static_cast< uint8_t >( numberOfMediaSetMembersV ) );

  return rawMediaInformation;
}

void ListFile::decodeMediaInformation( Helper::ConstRawDataSpan rawData )
{
  auto remaining{ rawData };

  // media set part number
  std::tie( remaining, mediaSetPnV ) = StringUtils_decodeString( remaining );

  // media sequence number
  uint8_t mediaSequenceNumber;
  std::tie( remaining, mediaSequenceNumber ) = Helper::RawData_getInt< uint8_t >( remaining );
  mediaSequenceNumberV = mediaSequenceNumber;

  // number of media set members
  uint8_t numberOfMediaSetMembers;
  std::tie( std::ignore, numberOfMediaSetMembers ) = Helper::RawData_getInt< uint8_t >( remaining );
  numberOfMediaSetMembersV = numberOfMediaSetMembers;
}

}
