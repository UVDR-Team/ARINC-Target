// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief ARINC 665 List Application.
 *
 * Utility to Scan Directories/ Files for ARINC 665 Content and Decodes them.
 **/

#include <arinc_665/Arinc665.hpp>

#include <arinc_665/files/FileListFile.hpp>
#include <arinc_665/files/LoadListFile.hpp>
#include <arinc_665/files/BatchListFile.hpp>
#include <arinc_665/files/LoadHeaderFile.hpp>
#include <arinc_665/files/BatchFile.hpp>

#include <arinc_665/utils/FilePrinter.hpp>

#include <arinc_665/Arinc665Exception.hpp>
#include <arinc_665/Version.hpp>

#include <helper/Dump.hpp>
#include <helper/Exception.hpp>
#include <helper/SeverityLevelDescription.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <format>

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
 * @brief Loads the load upload header file and decodes its content.
 *
 * @param[in] lubFile
 *   Load upload header.
 **/
static void printBatchFile( const std::filesystem::path &lubFile );

/**
 * @brief Loads the load upload header file and decodes its content.
 *
 * @param[in] luhFile
 *   Load upload header.
 **/
static void printLoadHeaderFile( const std::filesystem::path &luhFile );

/**
 * @brief Loads the load list file and decodes its content.
 *
 * @param[in] loadsLum
 *   Load list file.
 **/
static void printLoadListFile( const std::filesystem::path &loadsLum );

/**
 * @brief Loads the Batch list file and decodes its content.
 *
 * @param[in] filePath
 *   File Path.
 **/
static void printBatchListFile( const std::filesystem::path &filePath );

/**
 * @brief Loads the file list file and decodes its content.
 *
 * @param[in] filesLum
 *   File list file.
 **/
static void printFileListFile( const std::filesystem::path &filesLum );

/**
 * @brief Iterates over every file and subdirectory and tries to decode its
 *   content.
 *
 * @param[in] loadDir
 *   Directory to decode.
 **/
static void list_files( const std::filesystem::path &loadDir );

/**
 * @brief Load given @p file.
 *
 * @param[in] file
 *   Filename
 *
 * @return @p file a raw data.
 **/
static Helper::RawData loadFile( const std::filesystem::path &file );

int main( const int argc, char * argv[] )
try
{
  spdlog::set_level( spdlog::level::info );

  try
  {
    std::cout << std::format(
      "ARINC 665 list - {}\n",
      Arinc665::Version::VersionInformation );

    boost::program_options::options_description optionsDescription{ "ARINC 665 List options" };

    // directory to list
    std::filesystem::path directory;

    optionsDescription.add_options()
    (
      "help,h",
      "print this help screen"
    )
    (
      "log-level,l",
      boost::program_options::value< spdlog::level::level_enum >()
        ->default_value( spdlog::level::level_enum::warn, "warn" )
        ->value_name( "log-level" )
        ->notifier( []( const auto &logLevel ) {
          spdlog::set_level( logLevel );
        }),
      Helper::SeverityLevelDescription::instance().allLevels().c_str()
    )
    (
      "directory",
      boost::program_options::value( &directory )
        ->required()
        ->value_name( "directory" ),
      "start directory"
    );

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::parse_command_line( argc, argv, optionsDescription ),
      variablesMap );

    // NOLINTNEXTLINE( readability-container-contains ):VCPKG/MSVC compiles boost programm_options without C++20 support
    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout
        << "Prints the ARINC 665 Media File information located in the given directory\n"
        << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    std::cout << "List files in " << directory << "\n";

    list_files( directory );

    return EXIT_SUCCESS;
  }
  catch ( const boost::program_options::error &e )
  {
    std::cerr << std::format(
      "Error parsing command line: {}\n"
      "Enter '{} --help' for command line description.\n",
      e.what(),
      argv[ 0 ] );
    return EXIT_FAILURE;
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Error: {}\n", boost::diagnostic_information( e ) );
    return EXIT_FAILURE;
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Error: {}\n", boost::diagnostic_information( e ) );
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

static void printBatchFile( const std::filesystem::path &lubFile )
{
  try
  {
    const Arinc665::Files::BatchFile batch{ loadFile( lubFile ) };

    Arinc665::Utils::FilePrinter_print( batch, std::cout, "\t", "\t" );
  }
  catch ( const boost::exception &e )
  {
    std::cout << std::format( "Boost exception: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cout << std::format( "std exception: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cout << "unknown exception occurred\n";
  }
}

static void printLoadHeaderFile( const std::filesystem::path &luhFile )
{
  try
  {
    const auto rawLoadHeaderFile{ loadFile( luhFile ) };

    Arinc665::Files::LoadHeaderFile load{ rawLoadHeaderFile };

    Arinc665::Utils::FilePrinter_print( load, std::cout, "\t", "\t" );

    std::cout
      << std::format(
        "\tLoad CRC 0x{:02X}\n",
        Arinc665::Files::LoadHeaderFile::decodeLoadCrc( rawLoadHeaderFile ) );

    if ( load.loadCheckValueType() != Arinc649::CheckValueType::NotUsed )
    {
      std::cout
        << Arinc665::Files::LoadHeaderFile::decodeLoadCheckValue( rawLoadHeaderFile );
    }
  }
  catch ( const boost::exception &e )
  {
    std::cout << std::format( "Boost exception: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cout << std::format( "std exception: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cout << "unknown exception occurred\n";
  }
}

static void printLoadListFile( const std::filesystem::path &loadsLum )
{
  try
  {
    Arinc665::Utils::FilePrinter_print( Arinc665::Files::LoadListFile{ loadFile( loadsLum ) }, std::cout, "\t", "\t" );
  }
  catch ( const boost::exception &e )
  {
    std::cout << std::format( "Boost exception: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cout << std::format( "std exception: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cout << "unknown exception occurred\n";
  }
}

static void printBatchListFile( const std::filesystem::path &filePath )
{
  try
  {
    Arinc665::Utils::FilePrinter_print( Arinc665::Files::BatchListFile{ loadFile( filePath ) }, std::cout, "\t", "\t" );
  }
  catch ( const boost::exception &e )
  {
    std::cout << std::format( "Boost exception: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cout << std::format( "std exception: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cout << "unknown exception occurred\n";
  }
}

static void printFileListFile( const std::filesystem::path &filesLum )
{
  try
  {
    Arinc665::Utils::FilePrinter_print( Arinc665::Files::FileListFile{ loadFile( filesLum ) }, std::cout, "\t", "\t" );
  }
  catch ( const boost::exception &e )
  {
    std::cout << std::format( "Boost exception: {}\n", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    std::cout << std::format( "std exception: {}\n", e.what() );
  }
  catch ( ... )
  {
    std::cout << "unknown exception occurred\n";
  }
}

static void list_files( const std::filesystem::path &loadDir )
{
  for ( std::filesystem::directory_iterator itr( loadDir ); itr != std::filesystem::directory_iterator(); ++itr )
  {
    std::cout << itr->path() << " - ";

    if ( std::filesystem::is_directory( itr->path() ) )
    {
      std::cout << "Directory\n";
      list_files( itr->path());
    }
    else if ( is_regular_file( itr->status()))
    {
      if ( const auto fileType{ Arinc665::Files::Arinc665File::fileType( itr->path().filename() ) }; fileType )
      {
        switch ( *fileType )
        {
          using enum Arinc665::FileType;

          case BatchFile:
            std::cout << "ARINC 665 BATCH file\n";
            printBatchFile( itr->path() );
            break;

          case LoadUploadHeader:
            std::cout << "ARINC 665 LOAD UPLOAD HEADER file\n";
            printLoadHeaderFile( itr->path() );
            break;

          case LoadList:
            std::cout << "ARINC 665 LOAD LIST file\n";
            printLoadListFile( itr->path() );
            break;

          case BatchList:
            std::cout << "ARINC 665 BATCH LIST file\n";
            printBatchListFile( itr->path() );
            break;

          case FileList:
            std::cout << "ARINC 665 FILE LIST file\n";
            printFileListFile( itr->path() );
            break;

          default:
            std::cout << "INVALID VALUE\n";
            break;
        }
      }
      else
      {
        std::cout << "No special ARINC 665 file\n";
      }
    }
  }
}

static Helper::RawData loadFile( const std::filesystem::path &file )
{
  std::cout
    << std::format( "\tFile size is: {}\n", std::filesystem::file_size( file ) );

  Helper::RawData data( std::filesystem::file_size( file ) );

  std::ifstream fileStream{ file, std::ifstream::binary | std::ifstream::in };

  if ( !fileStream.is_open() )
  {
    BOOST_THROW_EXCEPTION(
      Arinc665::Arinc665Exception{}
        << Helper::AdditionalInfo{ "Error opening file" }
        << boost::errinfo_file_name{ file.string() } );
  }

  fileStream.read( reinterpret_cast< char * >( data.data() ), static_cast< std::streamsize >( data.size() ) );

  return data;
}
