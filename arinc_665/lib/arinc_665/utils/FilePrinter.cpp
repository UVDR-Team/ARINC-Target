// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Arinc665::Utils FilePrinter.
 **/

#include "FilePrinter.hpp"

#include <arinc_665/files/BatchFile.hpp>
#include <arinc_665/files/BatchListFile.hpp>
#include <arinc_665/files/FileListFile.hpp>
#include <arinc_665/files/LoadHeaderFile.hpp>
#include <arinc_665/files/LoadListFile.hpp>

#include <arinc_649/CheckValue.hpp>
#include <arinc_649/CheckValueTypeDescription.hpp>

#include <format>

namespace Arinc665::Utils {

void FilePrinter_print(
  const Files::FileListFile &fileListFile,
  std::ostream &outS,
  std::string_view initialIndent,
  std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  outS
    << std::format(
      "{0}media set PN: {1}\n"
      "{0}media seq no: {2}\n"
      "{0}no of media set members: {3}\n",
      initialIndent,
      fileListFile.mediaSetPn(),
      static_cast< std::string >( fileListFile.mediaSequenceNumber() ),
      static_cast< std::string >( fileListFile.numberOfMediaSetMembers() ) );

  for ( const auto &file : fileListFile.files() )
  {
    outS
      << std::format(
        "{0}Filename: {1}\n"
        "{0}File Path: {2}\n"
        "{0}File Member Sequence Number: {3}\n"
        "{0}File CRC: 0x{4:04X}\n"
        "{0}File Check Value: {5} {6}\n\n",
        nextIndent,
        file.filename,
        file.pathName,
        static_cast< std::string >( file.memberSequenceNumber ),
        file.crc,
        Arinc649::CheckValueTypeDescription::instance().name( file.checkValue.type() ),
        file.checkValue.toString() );
  }
}

void FilePrinter_print(
  const Files::LoadListFile &loadListFile,
  std::ostream &outS,
  std::string_view initialIndent,
  std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  outS
    << std::format(
      "{0}Media Set PN: {1}\n"
      "{0}Media seq no: {2}\n"
      "{0}no of media set members: {3}\n",
      initialIndent,
      loadListFile.mediaSetPn(),
      static_cast< std::string >( loadListFile.mediaSequenceNumber() ),
      static_cast< std::string >( loadListFile.numberOfMediaSetMembers() ) );

  for ( const auto & load : loadListFile.loads() )
  {
    outS
      << std::format(
        "\n{0}Load PN: {1}\n"
        "{0}Load Header filename: {2}\n"
        "{0}Load member sequence number: {3}\n",
        nextIndent,
        load.partNumber,
        load.headerFilename,
        static_cast< std::string >( load.memberSequenceNumber ) );

    for ( const auto & thw : load.targetHardwareIds )
    {
      outS << nextIndent << "Target Hardware ID: " << thw << "\n";
    }
  }
}

void FilePrinter_print(
  const Files::BatchListFile &batchListFile,
  std::ostream &outS,
  std::string_view initialIndent,
  std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  outS
    << std::format(
      "{0}Media Set PN: {1}\n"
      "{0}Media Seq no: {2}\n"
      "{0}no of media set members: {3}\n\n",
      initialIndent,
      batchListFile.mediaSetPn(),
      static_cast< std::string >( batchListFile.mediaSequenceNumber() ),
      static_cast< std::string >( batchListFile.numberOfMediaSetMembers() ) );

  for ( const auto & batch : batchListFile.batches() )
  {
    outS
      << std::format(
        "{0}Batch PN: {1}\n"
        "{0}Batch filename: {2}\n"
        "{0}Batch member sequence number: {3}\n\n",
        nextIndent,
        batch.partNumber,
        batch.filename,
        static_cast< std::string >( batch.memberSequenceNumber ) );
  }
}

void FilePrinter_print(
  const Files::LoadHeaderFile &loadHeaderFile,
  std::ostream &outS,
  std::string_view initialIndent,
  std::string_view indent )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  outS << initialIndent << "Part Number: " << loadHeaderFile.partNumber() << "\n";

  for ( const auto &targetHardwareId : loadHeaderFile.targetHardwareIds() )
  {
    outS << initialIndent << "Target HW ID: " << targetHardwareId << "\n";
  }

  for ( const auto &[ targetHardwareId, positions ] : loadHeaderFile.targetHardwareIdsPositions() )
  {
    outS << initialIndent << "Target HW ID: " << targetHardwareId << "\n";
    for ( const auto &position : positions )
    {
      outS << nextIndent << "Position: " << position << "\n";
    }
  }

  outS << "\n";

  for ( const auto &dataFile : loadHeaderFile.dataFiles() )
  {
    outS
      << std::format(
        "{0}Data File Filename: {1}\n"
        "{0}Data File PN: {2}\n"
        "{0}Data File Size: {3}\n"
        "{0}Data File CRC: 0x{4:02X}\n"
        "{0}Data File Check Value: {5} {6}\n\n",
        initialIndent,
        dataFile.filename,
        dataFile.partNumber,
        dataFile.length,
        dataFile.crc,
        Arinc649::CheckValueTypeDescription::instance().name( dataFile.checkValue.type() ),
        dataFile.checkValue.toString() );
  }

  for ( const auto &supportFile : loadHeaderFile.supportFiles() )
  {
    outS
      << std::format(
        "{0}Support File Filename: {1}\n"
        "{0}Support File PN: {2}\n"
        "{0}Support File Size: {3}\n"
        "{0}Support File CRC: 0x{4:02X}\n"
        "{0}Support File Check Value: {5} {6}\n\n",
        initialIndent,
        supportFile.filename,
        supportFile.partNumber,
        supportFile.length,
        supportFile.crc,
        Arinc649::CheckValueTypeDescription::instance().name( supportFile.checkValue.type() ),
        supportFile.checkValue.toString() );
  }
}

void FilePrinter_print(
  const Files::BatchFile &batchFile,
  std::ostream &outS,
  std::string_view initialIndent,
  std::string_view indent  )
{
  std::string nextIndent{ initialIndent };
  nextIndent += indent;

  outS << initialIndent << "Part Number: " << batchFile.partNumber() << "\n";
  outS << initialIndent << "Comment: "<< batchFile.comment() << "\n";

  for ( const auto &targetHardware : batchFile.targetsHardware() )
  {
    outS
      << initialIndent
      << "Target HW ID Position: " << targetHardware.targetHardwareIdPosition << "\n";
    for ( const auto &load : targetHardware.loads )
    {
      outS
        << nextIndent
        << "Load: " << load.headerFilename
        << " - " << load.partNumber << "\n";
    }
  }
}

}
