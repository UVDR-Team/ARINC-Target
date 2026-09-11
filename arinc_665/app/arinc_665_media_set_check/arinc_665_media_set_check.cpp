// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief ARINC 665 Media Set Validator.
 **/

#include <arinc_665/utils/MediaSetValidator.hpp>
#include <arinc_665/Arinc665Exception.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>

#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>

/**
 * @brief Application Entry Point.
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   Arguments
 *
 * @return Application exit status.
 **/
int main( int argc, char * argv[] );

/**
 * @brief Reads the give file and returns the data.
 *
 * @param[in] mediumNumber
 *   Medium number
 * @param[in] path
 *   Path of file on medium.
 *
 * @return Read file data.
 *
 * @throw Arinc665Exception
 *   If file does not exist or cannot be read.
 **/
static Helper::RawData readFile( uint8_t mediumNumber, const std::filesystem::path &path );

/**
 * @brief Print @p information
 *
 * @param information
 *   Information to print
 **/
static void printInformation( std::string_view information );

//! Media Directories
static std::vector< std::filesystem::path> mediaDirectories;

int main( const int argc, char * argv[] )
try
{
  spdlog::set_level( spdlog::level::info );

  try
  {
    std::cout << "ARINC 665 Media Set Validator\n";

    boost::program_options::options_description optionsDescription{ "ARINC 665 Media Set Validator Options" };

    optionsDescription.add_options()
    (
      "help",
      "print this help screen"
    )
    (
      "medium-directory",
      boost::program_options::value( &mediaDirectories )
        ->required()
        ->composing()
        ->value_name( "directory" ),
      "ARINC 665 medium source directory.\n"
      "For more media, repeat this parameter."
    );

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::parse_command_line( argc, argv, optionsDescription ),
      variablesMap );

    // NOLINTNEXTLINE( readability-container-contains ):VCPKG/MSVC compiles boost programm_options without C++20 support
    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout
        << "Validates ARINC 665 Media Set\n"
        << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    // create validator
    auto validator{ Arinc665::Utils::MediaSetValidator::create() };

    validator
      ->readFileHandler( std::bind_front( &readFile ) )
      .informationHandler( std::bind_front( &printInformation ) );

    // perform validation
    auto result{ (*validator)() };

    if ( !result )
    {
      std::cerr << "Validation FAILED\n";
      return EXIT_FAILURE;
    }

    std::cout << "Validation Successfully completed\n";

    return EXIT_SUCCESS;
  }
  catch ( const boost::program_options::error &e )
  {
    std::cerr
      << "Error parsing command line: " << e.what() << "\n"
      << "Enter " << argv[0]
      << " --help for command line description\n";
    return EXIT_FAILURE;
  }
  catch ( const boost::exception &e )
  {
    std::cerr
      << "Error: "
      << boost::diagnostic_information( e ) << "\n";
    return EXIT_FAILURE;
  }
  catch ( const std::exception &e )
  {
    std::cerr
      << "Error: "
      << boost::diagnostic_information( e ) << "\n";
    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    std::cerr << "Unknown exception occurred\n";
    return EXIT_FAILURE;
  }
}
catch ( ... )
{
  std::cerr << "Very bad exception\n";
  return EXIT_FAILURE;
}

static Helper::RawData readFile( const uint8_t mediumNumber, const std::filesystem::path &path )
{
  // check medium number
  if ( mediumNumber > mediaDirectories.size() )
  {
    return {};
  }

  auto filePath{ mediaDirectories[ mediumNumber-1 ] / path.relative_path() };

  // check existence of file
  if ( !std::filesystem::is_regular_file( filePath))
  {
    BOOST_THROW_EXCEPTION(
      Arinc665::Arinc665Exception()
        << boost::errinfo_file_name{ filePath.string() }
        << Helper::AdditionalInfo{ "File not found" } );
  }

  Helper::RawData data( std::filesystem::file_size( filePath ) );

  // load file
  std::ifstream file{ filePath.string().c_str(), std::ifstream::binary | std::ifstream::in };

  if ( !file.is_open() )
  {
    BOOST_THROW_EXCEPTION( Arinc665::Arinc665Exception()
      << Helper::AdditionalInfo{ "Error opening files" } );
  }

  // read the data to the buffer
  file.read( reinterpret_cast< char * >( data.data() ), static_cast< std::streamsize >( data.size() ) );

  // return the buffer
  return data;
}

static void printInformation( std::string_view information )
{
  std::cout << "Validation: " << information << "\n";
}
