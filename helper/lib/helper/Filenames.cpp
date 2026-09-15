// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Filenames.
 **/

#include "Filenames.hpp"

#include <array>
#include <algorithm>
#include <iterator>

namespace Helper {

//! Illegal filenames defined by Windows.
static constexpr auto illegalFilenames{ std::to_array< std::string_view >( {
  "CON",  "PRN",  "AUX",  "NUL",
  "COM0", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "COM¹", "COM²", "COM³",
  "LPT0", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9", "LPT¹", "LPT²", "LPT³" } ) };

//! Illegal filename characters.
static constexpr std::string_view illegalCharacters{
  "<"
  ">"
  ":"
  "\""
  "/"
  "\\"
  "|"
  "?"
  "*" };

std::string normaliseFilename( const std::string_view filename )
{
  // check for empty filename
  if ( filename.empty() )
  {
    return "_";
  }

  // check for "." and ".."
  if ( filename == "." )
  {
    return "_";
  }

  if ( filename == ".." )
  {
    return "__";
  }

  // check for illegal filenames (on Windows)
  if ( std::ranges::any_of(
    illegalFilenames,
    [ &filename ]( const std::string_view illegalFilename ) {
      return ( 0 == filename.compare( 0, filename.find( '.'), illegalFilename ) );
    } ) )
  {
    return std::string{ "_" }.append( filename );
  }

  // replace illegal characters by "_"
  std::string normalisedFilename;
  normalisedFilename.reserve( filename.size() );
  std::ranges::replace_copy_if(
    filename,
    std::back_inserter( normalisedFilename ),
    []( const char character ) {
      return illegalCharacters.find( character ) != std::string_view::npos;
    },
    '_' );

  return normalisedFilename;
}

}
