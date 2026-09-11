// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::MediaSetManagerImpl.
 **/

#include "MediaSetManagerImpl.hpp"

#include <ranges>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/File.hpp>
#include <arinc_665/media/Load.hpp>

#include <arinc_665/utils/FilesystemMediaSetDecompiler.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <utility>

namespace Arinc665::Utils {

MediaSetManagerImpl::MediaSetManagerImpl(
  std::filesystem::path directory,
  const bool checkFileIntegrity,
  const LoadProgressHandler &loadProgressHandler ) :
  directoryV{ std::move( directory ) }
{
  const auto configurationFile{ directoryV / ConfigurationFilename };

  if ( !std::filesystem::is_regular_file( configurationFile ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{}
      << Helper::AdditionalInfo{ "Media Set configuration file does not exists." }
      << boost::errinfo_file_name{ configurationFile.string() } );
  }

  boost::property_tree::ptree configurationProperties{};

  boost::property_tree::json_parser::read_json( configurationFile.string(), configurationProperties );

  auto configuration{ MediaSetManagerConfiguration{ configurationProperties } };

  loadMediaSets( configuration.mediaSets, checkFileIntegrity, std::move( loadProgressHandler ) );

  mediaSetDefaultsV = std::move( configuration.defaults );
}

MediaSetManagerImpl::~MediaSetManagerImpl()
{
  try
  {
    saveConfiguration();
  }
  catch ( const Arinc665Exception &e )
  {
    SPDLOG_CRITICAL( "Save configuration: {}", boost::diagnostic_information( e ) );
  }
}

const MediaSetDefaults& MediaSetManagerImpl::mediaSetDefaults() const
{
  return mediaSetDefaultsV;
}

MediaSetDefaults & MediaSetManagerImpl::mediaSetDefaults()
{
  return mediaSetDefaultsV;
}

void MediaSetManagerImpl::mediaSetDefaults( MediaSetDefaults mediaSetDefaults )
{
  mediaSetDefaultsV = std::move( mediaSetDefaults );
}

MediaSetManagerConfiguration MediaSetManagerImpl::configuration() const
{
  MediaSetManagerConfiguration configuration{};

  for ( const auto &mediaSetPaths : mediaSetsPathsV | std::views::values )
  {
    configuration.mediaSets.emplace_back( mediaSetPaths );
  }

  configuration.defaults = mediaSetDefaultsV;

  return configuration;
}

void MediaSetManagerImpl::saveConfiguration()
{
  try
  {
    boost::property_tree::write_json( ( directoryV / ConfigurationFilename ).string(), configuration().toProperties() );
  }
  catch ( const boost::property_tree::json_parser_error &e )
  {
    SPDLOG_ERROR( "Save configuration '{}': {}", e.filename(), e.message() );

    BOOST_THROW_EXCEPTION( Arinc665Exception{}
      << Helper::AdditionalInfo( e.message() )
      << boost::errinfo_file_name( e.filename() ) );
  }
  catch ( const boost::property_tree::ptree_error &e )
  {
    SPDLOG_ERROR( "Save configuration: {}", e.what() );

    BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ e.what() } );
  }
}

const std::filesystem::path& MediaSetManagerImpl::directory() const
{
  return directoryV;
}

bool MediaSetManagerImpl::hasMediaSet( const std::string_view partNumber ) const
{
  return mediaSetsInformationV.contains( partNumber );
}

std::optional< MediaSetInformation > MediaSetManagerImpl::mediaSet( const std::string_view partNumber ) const
{
  const auto mediaSet{ mediaSetsInformationV.find( partNumber ) };

  if ( mediaSet == mediaSetsInformationV.end() )
  {
    return {};
  }

  return mediaSet->second;
}

const MediaSetsInformation& MediaSetManagerImpl::mediaSets() const
{
  return mediaSetsInformationV;
}

void MediaSetManagerImpl::registerMediaSet( const MediaSetPaths &mediaSetPaths, const bool checkFileIntegrity )
{
  const auto decompiler( FilesystemMediaSetDecompiler::create() );
  assert( decompiler );

  // configure decompiler
  decompiler
    ->checkFileIntegrity( checkFileIntegrity )
    .mediaPaths( absoluteMediaPaths( mediaSetPaths ) );

  // import media set
  auto [ impMediaSet, checkValues ]{ ( *decompiler )() };
  assert( impMediaSet );

  if ( mediaSet( impMediaSet->partNumber() ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "Media Set already exist" } );
  }

  const std::string partNumber{ impMediaSet->partNumber() };

  // add to media sets information
  mediaSetsInformationV.try_emplace( partNumber, std::move( impMediaSet ), std::move( checkValues ) );

  // add to media sets paths
  mediaSetsPathsV.try_emplace( partNumber, mediaSetPaths );
}

MediaSetPaths MediaSetManagerImpl::deregisterMediaSet( std::string_view partNumber )
{
  // extract from media sets information
  if (
    const auto mediaSetInformation{ mediaSetsInformationV.extract( std::string{ partNumber } ) };
    !mediaSetInformation )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "Media Set not found" } );
  }

  // extract from media sets paths
  const auto mediaSetPaths{ mediaSetsPathsV.extract( std::string{ partNumber } ) };

  if ( !mediaSetPaths )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception{} << Helper::AdditionalInfo{ "Media Set paths not found" } );
  }

  // return the extracted paths information
  return mediaSetPaths.mapped();
}

Media::CheckValues MediaSetManagerImpl::checkValues( Media::ConstMediaSetPtr mediaSet )
{
  const auto mediaSetInformation{
    std::ranges::find( mediaSetsInformationV, mediaSet, []( const auto &value ) { return value.second.first; } ) };

  if ( mediaSetInformation == mediaSetsInformationV.end() )
  {
    return {};
  }

  return mediaSetInformation->second.second;
}

Media::ConstLoads MediaSetManagerImpl::loads() const
{
  Media::ConstLoads loads{};

  for ( const auto &[ mediaSet, checkValues ] : mediaSetsInformationV | std::views::values )
  {
    loads.splice( loads.end(), mediaSet->recursiveLoads() );
  }

  return loads;
}

Media::ConstBatches MediaSetManagerImpl::batches() const
{
  Media::ConstBatches batches{};

  for ( const auto &[ mediaSet, checkValues ] : mediaSetsInformationV | std::views::values )
  {
    batches.splice( batches.end(), mediaSet->recursiveBatches() );
  }

  return batches;
}

std::filesystem::path MediaSetManagerImpl::filePath( const Media::ConstFilePtr &file ) const
{
  if ( !file )
  {
    SPDLOG_ERROR( "Given file is empty" );
    return {};
  }

  const auto mediaSetIt{ mediaSetsPathsV.find( file->mediaSet()->partNumber() ) };

  if ( mediaSetIt == mediaSetsPathsV.end() )
  {
    SPDLOG_ERROR( "Media Set not found" );
    return {};
  }

  const auto mediumIt{ mediaSetIt->second.second.find( file->effectiveMediumNumber() ) };

  if ( mediumIt == mediaSetIt->second.second.end() )
  {
    SPDLOG_ERROR( "Medium not found" );
    return {};
  }

  return ( directoryV / mediaSetIt->second.first / mediumIt->second / file->path().relative_path() ).lexically_normal();
}

void MediaSetManagerImpl::loadMediaSets(
  const MediaSetManagerConfiguration::MediaSetsPaths &mediaSetsPaths,
  const bool checkFileIntegrity,
  const LoadProgressHandler &loadProgressHandler )
{
  for ( size_t mediaSetCounter{ 1U }; auto const &mediaSetPaths : mediaSetsPaths )
  {
    auto decompiler{ FilesystemMediaSetDecompiler::create() };
    assert( decompiler );

    // configure decompiler
    decompiler
      ->progressHandler( [&]( std::string_view partNumber, std::pair< MediumNumber, MediumNumber > medium )
      {
        if ( loadProgressHandler )
        {
          loadProgressHandler( { mediaSetCounter, mediaSetsPaths.size() }, partNumber, medium );
        }
      } )
      .checkFileIntegrity( checkFileIntegrity )
      .mediaPaths( absoluteMediaPaths( mediaSetPaths ) );

    // import media set
    auto [ impMediaSet, checkValues ]{ ( *decompiler )() };
    assert( impMediaSet );

    std::string partNumber{ impMediaSet->partNumber() };

    // add to media sets information
    mediaSetsInformationV.try_emplace( partNumber, std::move( impMediaSet ), std::move( checkValues ) );

    // add to media sets paths
    mediaSetsPathsV.try_emplace( partNumber, mediaSetPaths );

    // increment media set Index Counter
    ++mediaSetCounter;
  }
}

MediaPaths MediaSetManagerImpl::absoluteMediaPaths( const MediaSetPaths &mediaSetPaths ) const
{
  MediaPaths absoluteMediaPaths{};
  for ( const auto &[ mediumNumber, mediumPath ] : mediaSetPaths.second )
  {
    absoluteMediaPaths.try_emplace(
      mediumNumber,
      ( directoryV / mediaSetPaths.first / mediumPath ).lexically_normal() );
  }

  return absoluteMediaPaths;
}

}
