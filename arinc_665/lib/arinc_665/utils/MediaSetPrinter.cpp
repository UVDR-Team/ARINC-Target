// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc665::Utils Media Set Printer.
 **/

#include "MediaSetPrinter.hpp"

#include <arinc_665/media/Batch.hpp>
#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <arinc_665/MediumNumber.hpp>

#include <arinc_649/CheckValue.hpp>
#include <arinc_649/CheckValueTypeDescription.hpp>

#include <helper/Format.hpp>

namespace Arinc665::Utils {

/**
 * @brief Print Medium
 *
 * @param[in] mediaSet
 *   ARINC 665 Media Set
 * @param[in] mediumNumber
 *   Medium Number
 * @param[in,out] outS
 *   Output Stream
 * @param[in] initialIndent
 *   Initial Indention prepended before each output.
 * @param[in] indent
 *   Indent for sub-information
 **/
static void printMedium(
  const Media::MediaSet &mediaSet,
  const MediumNumber &mediumNumber,
  std::ostream &outS,
  std::string_view initialIndent,
  std::string_view indent );

/**
 * @brief Print Files of a given Container (Media Set Root or Directory) filtered to given Medium Number.
 *
 * @param[in] containerEntity
 *   Container to print the content from
 * @param[in] mediumNumber
 *   Medium Number filter
 * @param[in,out] outS
 *   Output Stream
 * @param[in] initialIndent
 *   Initial Indention prepended before each output.
 * @param[in] indent
 *   Indent for sub-information
 **/
static void printFiles(
  const Media::ContainerEntity &containerEntity,
  const MediumNumber &mediumNumber,
  std::ostream &outS,
  std::string_view initialIndent,
  std::string_view indent );

/**
 * @brief Converts the check value type to string representation
 *
 * @param[in] type
 *   Check Value Type
 *
 * @return Check Value Type as std::string.
 **/
static std::string printCheckValueType( std::optional< Arinc649::CheckValueType > type );

void MediaSetPrinter_print(
  const Media::MediaSet &mediaSet,
  std::ostream &outS,
  const std::string_view initialIndent,
  const std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  std::string nextNextIndent{ nextIndent };
  nextNextIndent += indent;

  outS
    << initialIndent
    << "Media Set Part Number: '" << mediaSet.partNumber() << "'\n"

    << initialIndent
    << "Number of Media: " << mediaSet.lastMediumNumber() << "\n"

    << initialIndent
    << "Media Set Check Value Type: "
    << printCheckValueType( mediaSet.mediaSetCheckValueType() )
    << " - "
    << printCheckValueType( mediaSet.effectiveMediaSetCheckValueType() ) << "\n"

    << initialIndent
    << "List of Files Check Value Type: "
    << printCheckValueType( mediaSet.listOfFilesCheckValueType() )
    << " - "
    << printCheckValueType( mediaSet.effectiveListOfFilesCheckValueType() ) << "\n"

    << initialIndent
    << "List of Loads Check Value Type: "
    << printCheckValueType( mediaSet.listOfLoadsCheckValueType() )
    << " - "
    << printCheckValueType( mediaSet.effectiveListOfLoadsCheckValueType() ) << "\n"

    << initialIndent
    << "List of Batches Check Value Type: "
    << printCheckValueType( mediaSet.listOfBatchesCheckValueType() )
    << " - "
    << printCheckValueType( mediaSet.effectiveListOfBatchesCheckValueType() ) << "\n"

    << initialIndent
    << "Files Check Value Type: "
    << printCheckValueType( mediaSet.filesCheckValueType() )
    << " - "
    << printCheckValueType( mediaSet.effectiveFilesCheckValueType() ) << "\n";

  // print files
  outS
    << initialIndent
    << "Media:" << "\n";

  // iterate over media
  for ( MediumNumber mediumNumber{ 1U }; mediumNumber <= mediaSet.lastMediumNumber(); ++mediumNumber )
  {
    printMedium( mediaSet, mediumNumber, outS, nextIndent, indent );
    outS << "\n";
  }

  // print Loads
  outS
    << initialIndent
    << "Loads:" << "\n";

  // iterate over loads
  for ( auto const &load : mediaSet.recursiveLoads() )
  {
    MediaSetPrinter_print( *load, outS, nextIndent, indent );
    outS << "\n";
  }

  if ( mediaSet.numberOfBatches() != 0U )
  {
    // iterate over Batches
    outS << initialIndent << "Batches:" << "\n";

    // iterate over loads
    for ( auto const &batch : mediaSet.recursiveBatches() )
    {
      MediaSetPrinter_print( *batch, outS, nextIndent, indent );
      outS << "\n";
    }
  }
}

void MediaSetPrinter_print( const Media::File &file, std::ostream &outS, const std::string_view initialIndent )
{
  outS
    << initialIndent
    << "File Name: "
    << "'" << file.name() << "'\n";

  outS
    << initialIndent
    << "File Path: "
    << "[" << file.effectiveMediumNumber() << "]:"
    << "'" << file.path().generic_string() << "'\n";

  outS
    << initialIndent
    << "File Type: ";

  switch ( file.fileType() )
  {
    using enum Arinc665::Media::FileType;

    case RegularFile:
      outS << "Regular File";
      break;

    case BatchFile:
      outS << "Batch File";
      break;

    case LoadFile:
      outS << "Load Header File";
      break;

    default:
      outS << "Illegal Value";
      break;
  }
  outS << "\n";

  outS
    << initialIndent
    << "File Check Value Type: "
    << printCheckValueType( file.checkValueType() )
    << " - "
    << printCheckValueType( file.effectiveCheckValueType() ) << "\n";

}

void MediaSetPrinter_print(
  const Media::Load &load,
  std::ostream &outS,
  const std::string_view initialIndent,
  const std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  std::string nextNextIndent{ nextIndent };
  nextNextIndent += indent;

  outS
    << initialIndent
    << "Load Name: "
    << load.name() << "\n"

    << initialIndent
    << "Load Path: "
    << "[" << load.effectiveMediumNumber() << "]:"
    << load.path().generic_string() << "\n"

    << initialIndent
    << "Load Part Number: '"
    << load.partNumber() << "'\n"

    << initialIndent
    << "Load Check Value Type: '"
    << printCheckValueType( load.loadCheckValueType() )
    << " - "
    << printCheckValueType( load.effectiveLoadCheckValueType() ) << "'\n"

    << initialIndent
    << "Data Files Check Value Type: '"
    << printCheckValueType( load.dataFilesCheckValueType() )
    << " - "
    << printCheckValueType( load.effectiveDataFilesCheckValueType() ) << "'\n"

    << initialIndent
    << "Support Files Check Value Type: '"
    << printCheckValueType( load.supportFilesCheckValueType() )
    << " - "
    << printCheckValueType( load.effectiveSupportFilesCheckValueType() ) << "'\n";

  if ( const auto &type{ load.loadType() }; type )
  {
    outS
      << initialIndent
      << "Load Type: '" << type->first << "'"
      << ARINC_FORMAT_NAMESPACE::format( "0x{:04X}", type->second ) << "\n";
  }

  outS
    << initialIndent
    << "Compatible THW IDs:\n";

  // iterate over THW ID list
  for ( auto const &[ thwId, positions ] : load.targetHardwareIdPositions() )
  {
    outS
      << nextIndent
      << "THW ID: " << thwId << "\n";

    for ( auto const &position : positions )
    {
      outS
        << nextNextIndent
        << "Position: " << position << "\n";
    }
    outS << "\n";
  }

  outS
    << initialIndent
    << "Data Files:\n";

  // iterate over Data Files
  for ( const auto &[ file, partNumber, checkValueType ] : load.dataFiles() )
  {
    outS
      << nextIndent
      << "Filename: "
      << file->name() << "\n"

      << nextIndent
      << "Path: "
      << "[" << file->effectiveMediumNumber() << "]:"
      << file->path().generic_string() << "\n"

      << nextIndent
      << "Part Number: '"
      << partNumber << "'\n"

      << nextIndent
      << "Check Value Type: '"
      << printCheckValueType( checkValueType )
      << " - "
      << printCheckValueType( checkValueType.value_or( load.effectiveDataFilesCheckValueType() ) ) << "'\n";

    outS << "\n";
  }

  outS
    << initialIndent
    << "Support Files:\n";

  // iterate over Support Files
  for ( const auto &[ file, partNumber, checkValueType ] : load.supportFiles() )
  {
    outS
      << nextIndent
      << "Filename: "
      << file->name() << "\n"

      << nextIndent
      << "Path: "
      << "[" << file->effectiveMediumNumber() << "]:"
      << file->path().generic_string() << "\n"

      << nextIndent
      << "Number: '"
      << partNumber << "'\n"

      << nextIndent
      << "Check Value Type: '"
      << printCheckValueType( checkValueType )
      << " - "
      << printCheckValueType( checkValueType.value_or( load.effectiveSupportFilesCheckValueType() ) ) << "'\n";

    outS << "\n";
  }
}

void MediaSetPrinter_print(
  const Media::Batch &batch,
  std::ostream &outS,
  const std::string_view initialIndent,
  const std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  std::string nextNextIndent{ nextIndent };
  nextNextIndent += indent;

  outS
    << initialIndent
    << "Filename: " << batch.name() << "\n"

    << initialIndent
    << "Path: "
    << "[" << batch.effectiveMediumNumber() << "]:"
    << batch.path().generic_string() << "\n"

    << initialIndent
    << "Part Number: '" << batch.partNumber() << "'\n"

    << initialIndent
    << "Comment: '" << batch.comment() << "'\n";

  outS
    << initialIndent
    << "Target Loads:\n";

  // iterate over Target list
  for ( auto const & [target,loads] : batch.targets() )
  {
    outS
      << nextIndent
      << "Target: " << target << "\n"
      << nextIndent
      << "Loads: " << "\n";

    for ( const auto &load : loads )
    {
      outS
        << nextNextIndent
        << "Name: " << load->name() << "\n"

        << nextNextIndent
        << "Path: "
        << "[" << load->effectiveMediumNumber() << "]:"
        << load->path().generic_string() << "\n";

      outS << "\n";
    }

    outS << "\n";
  }
}

void MediaSetPrinter_print(
  const Media::CheckValues &checkValues,
  std::ostream &outS,
  const std::string_view initialIndent,
  const std::string_view indent )
{
  for ( const auto &[ file, checkValuesSet ] : checkValues )
  {
    outS << ARINC_FORMAT_NAMESPACE::format( "{}{}\n", initialIndent, file->path().string() );

    for ( const auto &checkValue : checkValuesSet )
    {
      outS << ARINC_FORMAT_NAMESPACE::format( "{}{}{}\n", initialIndent, indent, checkValue );
    }
  }
}

static void printMedium(
  const Media::MediaSet &mediaSet,
  const MediumNumber &mediumNumber,
  std::ostream &outS,
  const std::string_view initialIndent,
  const std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  outS
    << initialIndent
    << "Medium Number: "
    << mediumNumber << "\n";

  outS
    << initialIndent
    << "Files:\n";

  printFiles( mediaSet, mediumNumber, outS, initialIndent, indent );
}

static void printFiles(
  const Media::ContainerEntity &containerEntity,
  const MediumNumber &mediumNumber,
  std::ostream &outS,
  std::string_view initialIndent,
  std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  // iterate over files
  for ( auto const &file : containerEntity.files( mediumNumber ) )
  {
    MediaSetPrinter_print( *file, outS, nextIndent );
    outS << "\n";
  }

  // iterate over subdirectories
  for ( const auto &container : containerEntity.subdirectories( mediumNumber ) )
  {
    printFiles( *container, mediumNumber, outS, initialIndent, indent );
  }
}

static std::string printCheckValueType( std::optional< Arinc649::CheckValueType > type )
{
  return ( type ? std::string( Arinc649::CheckValueTypeDescription::instance().name( *type ) ) : "***Undefined***" );
}

}
