// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Media::Batch.
 **/

#include "Batch.hpp"

namespace Arinc665::Media {

Batch::Batch(
  const ContainerEntityPtr &parent,
  std::string name,
  const OptionalMediumNumber &mediumNumber,
  [[maybe_unused]] const CreateKey &createKey ) :
  File{ parent, std::move( name ), mediumNumber }
{
}

FileType Batch::fileType() const
{
  return FileType::BatchFile;
}

std::string_view Batch::partNumber() const
{
  return partNumberV;
}

void Batch::partNumber( std::string partNumber )
{
  partNumberV = std::move( partNumber );
}

std::string_view Batch::comment() const
{
  return commentV;
}

void Batch::comment( std::string comment )
{
  commentV = std::move( comment );
}

ConstBatchInformation Batch::targets() const
{
  ConstBatchInformation batchInfo{};

  for ( const auto &[ targetHardwareId, loads ] : batchesV )
  {
    batchInfo.try_emplace( targetHardwareId, loads.begin(), loads.end() );
  }

  return batchInfo;
}

ConstLoads Batch::target( std::string_view targetHardwareIdPosition ) const
{
  const auto targetLoads{ batchesV.find( targetHardwareIdPosition ) };

  if ( targetLoads == batchesV.end() )
  {
    return {};
  }

  return { targetLoads->second.begin(), targetLoads->second.end() };
}

void Batch::target( std::string targetHardwareIdPosition, const ConstLoads &loads )
{
  batchesV.try_emplace( std::move( targetHardwareIdPosition ), loads.begin(), loads.end() );
}

void Batch::target( std::string_view targetHardwareIdPosition, const ConstLoadPtr &load )
{
  batchesV[ std::string{ targetHardwareIdPosition } ].emplace_back( load );
}

}
