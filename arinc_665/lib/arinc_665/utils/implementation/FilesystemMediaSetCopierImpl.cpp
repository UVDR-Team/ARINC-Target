// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::FilesystemMediaSetCopierImpl.
 **/

#include "FilesystemMediaSetCopierImpl.hpp"

#include <arinc_665/files/MediaSetInformation.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <format>

namespace Arinc665::Utils {

FilesystemMediaSetCopier& FilesystemMediaSetCopierImpl::mediaPaths( MediaPaths mediaPaths )
{
  mediaPathsV = std::move( mediaPaths );
  return * this;
}

FilesystemMediaSetCopier &FilesystemMediaSetCopierImpl::outputBasePath( std::filesystem::path outputBasePath )
{
  outputBasePathV = std::move( outputBasePath );
  return *this;
}

FilesystemMediaSetCopier &FilesystemMediaSetCopierImpl::mediaSetName( std::string mediaSetName )
{
  mediaSetNameV = std::move( mediaSetName );
  return *this;
}

MediaSetPaths FilesystemMediaSetCopierImpl::operator()()
{
  if ( mediaPathsV.empty() || outputBasePathV.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
      << Helper::AdditionalInfo{ "Not all parameter provided" } );
  }

  // update name to part number if not provided
  if ( mediaSetNameV.empty() )
  {
    auto mediumInformation{ getMediumInformation( mediaPathsV.begin()->second ) };

    if ( mediumInformation )
    {
      mediaSetNameV = mediumInformation->partNumber;
    }
  }

  const auto mediaSetBasePath{ outputBasePathV / mediaSetNameV };

  // Create the output directory
  if ( std::error_code err{};
    !std::filesystem::create_directories( mediaSetBasePath, err ) || err )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception{}
      << Helper::AdditionalInfo{ err.message() }
      << boost::errinfo_file_name{ mediaSetBasePath.string() } );
  }

  // iterate over media
  MediaPaths destinationMediaPaths{};
  for ( auto const &[ mediumNumber, mediumPath ] : mediaPathsV )
  {
    auto destinationMediumDir{ std::format( "MEDIUM_{:03d}", static_cast< uint8_t >( mediumNumber ) ) };

    // copy medium
    std::filesystem::copy(
      mediumPath,
      mediaSetBasePath / destinationMediumDir,
      std::filesystem::copy_options::recursive );

    // store medium destination path
    destinationMediaPaths.try_emplace( mediumNumber, destinationMediumDir );
  }

  return { mediaSetNameV, destinationMediaPaths };
}

}
