// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of namespace Arinc665::Utils.
 **/

#include "Utils.hpp"

#include <arinc_665/files/FileListFile.hpp>
#include <arinc_665/files/MediaSetInformation.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/diagnostic_information.hpp>

#include <fstream>

namespace Arinc665::Utils {

std::optional< Files::MediaSetInformation > getMediumInformation( const std::filesystem::path &directory )
{
  try
  {
    // Check the existence of the directory
    if ( !std::filesystem::is_directory( directory ) )
    {
      SPDLOG_WARN( "'{}' is not a directory", directory.string() );
      return {};
    }

    const auto fileListFilePath{ directory / ListOfFilesName };

    // check existence of list of files
    if ( !std::filesystem::is_regular_file( fileListFilePath ) )
    {
      SPDLOG_WARN( "'{}' is not a regular file", fileListFilePath.string() ) ;
      return {};
    }

    const auto fileSize{ std::filesystem::file_size( fileListFilePath ) };

    std::ifstream file{ fileListFilePath.string().c_str(), std::ifstream::binary | std::ifstream::in };

    if ( !file.is_open() )
    {
      SPDLOG_WARN( "Error opening file '{}'", fileListFilePath. string() );
      return {};
    }

    Helper::RawData rawFile( fileSize );
    file.read( reinterpret_cast< char * >( std::data( rawFile ) ), static_cast< std::streamsize >( fileSize ) );

    if ( file.bad() || ( file.gcount() != static_cast< std::streamsize >( fileSize ) ) )
    {
      SPDLOG_WARN( "Error reading from file '{}' read {} bytes", fileListFilePath.string(), file.gcount() );
      return {};
    }

    const Files::FileListFile fileListFile{ rawFile };

    return fileListFile.mediaSetInformation();
  }
  catch ( const boost::exception &e )
  {
    SPDLOG_WARN( "Exception {}", boost::diagnostic_information( e, false ) );
    SPDLOG_INFO( "Exception Details {}", boost::diagnostic_information( e ) );

    return {};
  }
}

}
