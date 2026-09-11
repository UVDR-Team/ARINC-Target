// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Media::Load.
 **/

#include "Load.hpp"

#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/CheckValue.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <utility>

namespace Arinc665::Media {

Load::Load(
  const ContainerEntityPtr &parent,
  std::string name,
  const OptionalMediumNumber &mediumNumber,
  [[maybe_unused]] const CreateKey &createKey ) :
  File{ parent, std::move( name ), mediumNumber }
{
}

FileType Load::fileType() const
{
  return FileType::LoadFile;
}

uint16_t Load::partFlags() const
{
  return partFlagsV;
}

void Load::partFlags( const uint16_t partFlags )
{
  partFlagsV = partFlags;
}

std::string_view Load::partNumber() const
{
  return partNumberV;
}

void Load::partNumber( std::string partNumber )
{
  partNumberV = std::move( partNumber );
}

const Load::TargetHardwareIdPositions& Load::targetHardwareIdPositions() const
{
  return targetHardwareIdPositionsV;
}

Load::TargetHardwareIdPositions& Load::targetHardwareIdPositions()
{
  return targetHardwareIdPositionsV;
}

void Load::targetHardwareIdPositions( TargetHardwareIdPositions targetHardwareIdPositions )
{
  targetHardwareIdPositionsV = std::move( targetHardwareIdPositions );
}

Load::TargetHardwareIds Load::targetHardwareIds() const
{
  TargetHardwareIds thwIds{};

  for ( const auto &[ thwId, positions ] : targetHardwareIdPositionsV )
  {
    thwIds.emplace( thwId );
  }

  return thwIds;
}

void Load::targetHardwareIds( const TargetHardwareIds &thwIds )
{
  for ( const auto &targetHardwareId : thwIds )
  {
    targetHardwareIdPositionsV.insert_or_assign( targetHardwareId, Positions{} );
  }
}

void Load::targetHardwareId( std::string targetHardwareId, Positions positions )
{
  targetHardwareIdPositionsV.insert_or_assign( std::move( targetHardwareId ), std::move( positions ) );
}

ConstFilePtr Load::file( const std::string_view filename ) const
{
  ConstFiles foundFiles{};

  for ( const auto &file : files() )
  {
    if ( file->name() == filename )
    {
      foundFiles.emplace_back( file );
    }
  }

  if ( 1U == foundFiles.size() )
  {
    return foundFiles.front();
  }

  if ( !foundFiles.empty() )
  {
    SPDLOG_INFO( "More than one file found for the given filename" );
  }

  return {};
}

ConstFilePtr Load::file(
  const CheckValues &checkValues,
  const std::string_view filename,
  const Arinc649::CheckValue &checkValue ) const
{
  // if no check value is provided, search for filename only
  if ( Arinc649::CheckValue::NoCheckValue == checkValue )
  {
    return file( filename );
  }

  ConstFiles foundFiles{};

  for ( const auto &file : files() )
  {
    if ( file->name() == filename )
    {
      if ( auto fileCheckValues{ checkValues.find( file ) };
        ( fileCheckValues != checkValues.end() ) && fileCheckValues->second.contains( checkValue ) )
      {
        foundFiles.emplace_back( file );
      }
    }
  }

  if ( 1U == foundFiles.size() )
  {
    return foundFiles.front();
  }

  if ( !foundFiles.empty() )
  {
    SPDLOG_INFO( "More than one file was found for given parameters" );
  }

  return {};

}

ConstFiles Load::files() const
{
  // add the load file itself
  ConstFiles files{ {
    std::dynamic_pointer_cast< const File >( shared_from_this() ) } };

  // add data files
  for ( const auto &[ file, filePartNumber, checkValueType ] : dataFilesV )
  {
    files.emplace_back( file );
  }

  // add support files
  for ( const auto &[ file, filePartNumber, checkValueType ] : supportFilesV )
  {
    files.emplace_back( file );
  }

  return files;
}

ConstLoadFiles Load::dataFiles( const bool effective ) const
{
  ConstLoadFiles files{};

  for ( const auto &[ filePtr, partNumber, checkValueType ] : dataFilesV )
  {
    if ( effective )
    {
      files.emplace_back(
        filePtr.lock(),
        partNumber,
        checkValueType.value_or( effectiveDataFilesCheckValueType() ) );
    }
    else
    {
      files.emplace_back( filePtr.lock(), partNumber, checkValueType );
    }
  }

  return files;
}

void Load::dataFiles( const ConstLoadFiles &files )
{
  dataFilesV.assign( files.begin(), files.end() );
}

void Load::dataFile(
  const ConstRegularFilePtr &file,
  std::string partNumber,
  std::optional< Arinc649::CheckValueType > checkValueType )
{
  if ( !file || ( file->mediaSet() != mediaSet() ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{}
      << Helper::AdditionalInfo{ "invalid File" } );
  }

  dataFilesV.emplace_back(
    file,
    std::move( partNumber ),
    checkValueType );
}

ConstLoadFiles Load::supportFiles( const bool effective ) const
{
  ConstLoadFiles files{};

  for ( const auto &[filePtr, partNumber, checkValueType ] : supportFilesV )
  {
    if ( effective )
    {
      files.emplace_back(
        filePtr.lock(),
        partNumber,
        checkValueType.value_or( effectiveSupportFilesCheckValueType() ) );
    }
    else
    {
      files.emplace_back( filePtr.lock(), partNumber, checkValueType );
    }
  }

  return files;
}

void Load::supportFiles( const ConstLoadFiles &files )
{
  supportFilesV.assign( files.begin(), files.end() );
}

void Load::supportFile(
  const ConstRegularFilePtr &file,
  std::string partNumber,
  std::optional< Arinc649::CheckValueType > checkValueType )
{
  if ( !file || ( file->mediaSet() != mediaSet() ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{}
      << Helper::AdditionalInfo{ "invalid File" } );
  }

  supportFilesV.emplace_back( file, std::move( partNumber ), checkValueType );
}

Helper::ConstRawDataSpan Load::userDefinedData() const
{
  return userDefinedDataV;
}

Helper::RawData& Load::userDefinedData()
{
  return userDefinedDataV;
}

void Load::userDefinedData( Helper::RawData userDefinedData )
{
  userDefinedDataV = std::move( userDefinedData ) ;
}

const Load::Type& Load::loadType() const
{
  return typeV;
}

void Load::loadType( Type type )
{
  typeV = std::move( type );
}

Arinc649::CheckValueType Load::effectiveLoadCheckValueType() const
{
  return loadCheckValueTypeV.value_or(
    mediaSet()->mediaSetCheckValueType().value_or(
      Arinc649::CheckValueType::NotUsed ) );
}

std::optional< Arinc649::CheckValueType > Load::loadCheckValueType() const
{
  return loadCheckValueTypeV;
}

void Load::loadCheckValueType(
  std::optional< Arinc649::CheckValueType > checkValueType )
{
  loadCheckValueTypeV = checkValueType;
}

Arinc649::CheckValueType Load::effectiveDataFilesCheckValueType() const
{
  return dataFilesCheckValueTypeV.value_or(
    mediaSet()->effectiveMediaSetCheckValueType() );
}

std::optional< Arinc649::CheckValueType > Load::dataFilesCheckValueType() const
{
  return dataFilesCheckValueTypeV;
}

void Load::dataFilesCheckValueType( std::optional< Arinc649::CheckValueType > checkValueType )
{
  dataFilesCheckValueTypeV = checkValueType;
}

Arinc649::CheckValueType Load::effectiveSupportFilesCheckValueType() const
{
  return supportFilesCheckValueTypeV.value_or( mediaSet()->effectiveMediaSetCheckValueType() );
}

std::optional< Arinc649::CheckValueType > Load::supportFilesCheckValueType() const
{
  return supportFilesCheckValueTypeV;
}

void Load::supportFilesCheckValueType( std::optional< Arinc649::CheckValueType > checkValueType )
{
  supportFilesCheckValueTypeV = checkValueType;
}

ConstLoadPtr Loads_loadByPartNumber( const ConstLoads &loads, std::string_view partNumber )
{
  for ( const auto &load : loads )
  {
    if ( load->partNumber() == partNumber )
    {
      return load;
    }
  }

  return {};
}

ConstFilePtr Loads_file( const ConstLoads &loads, std::string_view filename, std::string_view loadPartNumber )
{
  if ( !loadPartNumber.empty() )
  {
    const auto load{ Loads_loadByPartNumber( loads, loadPartNumber ) };

    if ( !load )
    {
      SPDLOG_ERROR( "No Load with given Part Number" );
      return {};
    }

    return load->file( filename );
  }

  ConstFiles files{};

  for ( const auto &load : loads )
  {
    assert( load );
    if ( auto file{ load->file( filename ) }; file )
    {
      files.emplace_back( std::move( file ) );
    }
  }

  if ( 1U == files.size() )
  {
    return files.front();
  }

  if ( !files.empty() )
  {
    SPDLOG_INFO( "More than one file found for given parameters" );
  }

  return {};
}

ConstFilePtr Loads_file(
  const ConstLoads &loads,
  const CheckValues &checkValues,
  const std::string_view filename,
  const std::string_view loadPartNumber,
  const Arinc649::CheckValue &checkValue )
{
  if ( !loadPartNumber.empty() )
  {
    const auto load{ Loads_loadByPartNumber( loads, loadPartNumber ) };

    if ( !load )
    {
      SPDLOG_ERROR( "No Load with the given Part Number" );
      return {};
    }

    SPDLOG_INFO( "Look for a file within the load: {}", load->name() );
    return load->file( checkValues, filename, checkValue );
  }

  ConstFiles files{};

  for ( const auto &load : loads )
  {
    assert( load );
    if ( auto file{ load->file( checkValues, filename, checkValue ) }; file )
    {
      files.emplace_back( std::move( file ) );
    }
  }

  if ( 1U == files.size() )
  {
    return files.front();
  }

  if ( !files.empty() )
  {
    SPDLOG_INFO( "More than one file was found for given parameters" );
  }

  // no, or more than one file
  return {};
}

}
