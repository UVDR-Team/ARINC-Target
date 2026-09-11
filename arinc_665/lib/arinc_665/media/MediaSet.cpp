// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Media::MediaSet.
 **/

#include "MediaSet.hpp"

#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/Batch.hpp>

namespace Arinc665::Media {

MediaSetPtr MediaSet::create()
{
  return std::make_shared< MediaSet >( CreateKey{} );
}

MediaSet::MediaSet( [[maybe_unused]] const CreateKey &createKey ) :
  ContainerEntity{ MediumNumber{ 1U } }
{
}

ConstMediaSetPtr MediaSet::mediaSet() const
{
  return std::dynamic_pointer_cast< const MediaSet >( shared_from_this() );
}

MediaSetPtr MediaSet::mediaSet()
{
  return std::dynamic_pointer_cast< MediaSet >( shared_from_this() );
}

Type MediaSet::type() const
{
  return Type::MediaSet;
}

ConstContainerEntityPtr MediaSet::parent() const
{
  return {};
}

ContainerEntityPtr MediaSet::parent()
{
  return {};
}

std::filesystem::path MediaSet::path() const
{
  return std::filesystem::path{ { std::filesystem::path::preferred_separator } };
}

std::string_view MediaSet::partNumber() const
{
  return partNumberV;
}

void MediaSet::partNumber( std::string partNumber )
{
  partNumberV = std::move( partNumber );
}

ConstLoads MediaSet::loadsWithFile( const ConstRegularFilePtr &file ) const
{
  ConstLoads foundLoads{};

  for ( const auto &load : recursiveLoads() )
  {
    bool loadAdded{ false };
    for ( const auto &[ dataFile, partNumber, checkValue ] : load->dataFiles() )
    {
      if ( dataFile == file )
      {
        foundLoads.emplace_back( load );
        loadAdded = true;
        break;
      }
    }

    if ( loadAdded )
    {
      break ;
    }

    for ( const auto &[ supportFile, partNumber, checkValue ] : load->supportFiles() )
    {
      if ( supportFile == file )
      {
        foundLoads.emplace_back( load );
      }
    }
  }

  return foundLoads;
}

ConstBatches MediaSet::batchesWithLoad( const ConstLoadPtr &load ) const
{
  ConstBatches foundBatches{};

  for ( const auto & batch : recursiveBatches() )
  {
    for ( const auto &[ thwIdPos, loads ]: batch->targets() )
    {
      if ( loads.end() != std::ranges::find( loads, load ) )
      {
        foundBatches.emplace_back( batch );
      }
    }
  }

  return foundBatches;
}

Helper::ConstRawDataSpan MediaSet::filesUserDefinedData() const
{
  return filesUserDefinedDataV;
}

Helper::RawData& MediaSet::filesUserDefinedData()
{
  return filesUserDefinedDataV;
}

void MediaSet::filesUserDefinedData( Helper::RawData userDefinedData )
{
  filesUserDefinedDataV = std::move( userDefinedData );
}

Helper::ConstRawDataSpan MediaSet::loadsUserDefinedData() const
{
  return loadsUserDefinedDataV;
}

Helper::RawData& MediaSet::loadsUserDefinedData()
{
  return loadsUserDefinedDataV;
}

void MediaSet::loadsUserDefinedData( Helper::RawData userDefinedData )
{
  loadsUserDefinedDataV = std::move( userDefinedData );
}

Helper::ConstRawDataSpan MediaSet::batchesUserDefinedData() const
{
  return batchesUserDefinedDataV;
}

Helper::RawData& MediaSet::batchesUserDefinedData()
{
  return batchesUserDefinedDataV;
}

void MediaSet::batchesUserDefinedData( Helper::RawData userDefinedData )
{
  batchesUserDefinedDataV = std::move( userDefinedData );
}

Arinc649::CheckValueType MediaSet::effectiveMediaSetCheckValueType() const
{
  return mediaSetCheckValueTypeV.value_or( Arinc649::CheckValueType::NotUsed );
}

std::optional< Arinc649::CheckValueType >
MediaSet::mediaSetCheckValueType() const
{
  return mediaSetCheckValueTypeV;
}

void MediaSet::mediaSetCheckValueType( const std::optional< Arinc649::CheckValueType > type )
{
  mediaSetCheckValueTypeV = type;
}

Arinc649::CheckValueType MediaSet::effectiveListOfFilesCheckValueType() const
{
  return listOfFilesCheckValueTypeV.value_or( mediaSetCheckValueTypeV.value_or( Arinc649::CheckValueType::NotUsed ) );
}

std::optional< Arinc649::CheckValueType > MediaSet::listOfFilesCheckValueType() const
{
  return listOfFilesCheckValueTypeV;
}

void MediaSet::listOfFilesCheckValueType( const std::optional< Arinc649::CheckValueType > type )
{
  listOfFilesCheckValueTypeV = type;
}

Arinc649::CheckValueType MediaSet::effectiveListOfLoadsCheckValueType() const
{
  return listOfLoadsCheckValueTypeV.value_or( effectiveFilesCheckValueType() );
}

std::optional< Arinc649::CheckValueType > MediaSet::listOfLoadsCheckValueType() const
{
  return listOfLoadsCheckValueTypeV;
}

void MediaSet::listOfLoadsCheckValueType( const std::optional< Arinc649::CheckValueType > type )
{
  listOfLoadsCheckValueTypeV = type;
}

Arinc649::CheckValueType MediaSet::effectiveListOfBatchesCheckValueType() const
{
  return listOfBatchesCheckValueTypeV.value_or( effectiveFilesCheckValueType() );
}

std::optional< Arinc649::CheckValueType > MediaSet::listOfBatchesCheckValueType() const
{
  return listOfBatchesCheckValueTypeV;
}

void MediaSet::listOfBatchesCheckValueType( const std::optional< Arinc649::CheckValueType > type )
{
  listOfBatchesCheckValueTypeV = type;
}

Arinc649::CheckValueType MediaSet::effectiveFilesCheckValueType() const
{
  return filesCheckValueTypeV.value_or( effectiveMediaSetCheckValueType() );
}

std::optional< Arinc649::CheckValueType > MediaSet::filesCheckValueType() const
{
  return filesCheckValueTypeV;
}

void MediaSet::filesCheckValueType( const std::optional< Arinc649::CheckValueType > type )
{
  filesCheckValueTypeV = type;
}

}
