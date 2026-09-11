// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::FilesystemMediaSetRemoverImpl.
 **/

#include "FilesystemMediaSetRemoverImpl.hpp"

#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

namespace Arinc665::Utils {

FilesystemMediaSetRemover& FilesystemMediaSetRemoverImpl::mediaSetPaths( MediaSetPaths mediaSetPaths )
{
  mediaSetPathsV = std::move( mediaSetPaths );
  return * this;
}

void FilesystemMediaSetRemoverImpl::operator()()
{
  if ( mediaSetPathsV.second.empty() )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "No Media Set Paths provided" } );
  }

  // iterate over media
  for ( auto const &[ mediumNumber, mediumPath ] : mediaSetPathsV.second )
  {
    std::filesystem::remove_all( mediaSetPathsV.first / mediumPath );
  }

  if ( !mediaSetPathsV.first.empty() )
  {
    // remove media set directories
    std::filesystem::remove_all( mediaSetPathsV.first );
  }
}

}
