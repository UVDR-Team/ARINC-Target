// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::Arinc665XmlSaveImpl.
 **/

#include "Arinc665XmlSaveImpl.hpp"

#include <arinc_665/media/Batch.hpp>
#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/CheckValueTypeDescription.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <helper/Format.hpp>

namespace Arinc665::Utils {

Arinc665XmlSaveImpl::Arinc665XmlSaveImpl(
  const Media::MediaSet &mediaSet,
  const FilePathMapping &filePathMapping,
  const std::filesystem::path &xmlFile ) :
  mediaSetV{ mediaSet },
  filePathMappingV{ filePathMapping },
  xmlFileV{ xmlFile }
{
}

void Arinc665XmlSaveImpl::operator()()
{
  SPDLOG_INFO( "Save Media Set '{}' to '{}'", mediaSetV.partNumber(), xmlFileV.string() );

  try
  {
    xmlpp::Document xmlDoc{};
    auto * const mediaSetNode{ xmlDoc.create_root_node( "MediaSet" ) };

    mediaSet( *mediaSetNode );

    xmlDoc.write_to_file_formatted( xmlFileV.string() );
  }
  catch ( const xmlpp::exception &e )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ e.what() }
      << boost::errinfo_file_name{ xmlFileV.string() } );
  }
}

std::string Arinc665XmlSaveImpl::userDefinedData( Helper::ConstRawDataSpan userDefinedData )
{
  return std::string{ reinterpret_cast< char const * >( userDefinedData.data() ), userDefinedData.size() };
}

void Arinc665XmlSaveImpl::mediaSet( xmlpp::Element &mediaSetElement ) const
{
  mediaSetElement.set_attribute( "PartNumber", std::string( mediaSetV.partNumber() ) );

  // Media Set Check Value
  checkValue( mediaSetElement, "MediaSetCheckValue", mediaSetV.mediaSetCheckValueType() );

  // List of Files Check Value
  checkValue( mediaSetElement, "ListOfFilesCheckValue", mediaSetV.listOfFilesCheckValueType() );

  // List of Loads Check Value
  checkValue( mediaSetElement, "ListOfLoadsCheckValue", mediaSetV.listOfLoadsCheckValueType() );

  // List of Batches Check Value
  checkValue( mediaSetElement, "ListOfBatchesCheckValue", mediaSetV.listOfBatchesCheckValueType() );

  // Files Check Value
  checkValue( mediaSetElement, "FilesCheckValue", mediaSetV.filesCheckValueType() );

  // Files List User Defined Data
  if ( const auto &filesUserDefinedData{ mediaSetV.filesUserDefinedData() }; !filesUserDefinedData.empty() )
  {
    mediaSetElement.add_child_element( "FilesUserDefinedData" )
      ->add_child_text( userDefinedData( filesUserDefinedData ) );
  }

  // List of Loads User Defined Data
  if ( const auto &loadsUserDefinedData{ mediaSetV.loadsUserDefinedData() }; !loadsUserDefinedData.empty() )
  {
    mediaSetElement.add_child_element( "LoadsUserDefinedData" )
      ->add_child_text( userDefinedData( loadsUserDefinedData ) );
  }

  // List of Batches User Defined Data
  if ( const auto &batchesUserDefinedData{ mediaSetV.batchesUserDefinedData() }; !batchesUserDefinedData.empty() )
  {
    mediaSetElement.add_child_element( "BatchesUserDefinedData" )
      ->add_child_text( userDefinedData( batchesUserDefinedData ) );
  }

  // Content
  auto * const contentNode{ mediaSetElement.add_child_element( "Content" ) };
  entries( mediaSetV, *contentNode );
}

void Arinc665XmlSaveImpl::entries(
  const Media::ContainerEntity &currentContainer,
  xmlpp::Element &currentContainerElement ) const
{
  // set default medium if provided
  if ( const auto defaultMedium{ currentContainer.defaultMediumNumber() }; defaultMedium )
  {
    currentContainerElement.set_attribute(
      "DefaultMedium",
      std::to_string( static_cast< uint8_t >( *defaultMedium ) ) );
  }

  // iterate over subdirectories within container and add them recursively
  for ( const auto &dirEntry : currentContainer.subdirectories() )
  {
    auto * const directoryNode{ currentContainerElement.add_child_element( "Directory" ) };

    directoryNode->set_attribute( "Name", std::string{ dirEntry->name() } );

    entries( *dirEntry, *directoryNode );
  }

  // iterate over files within current container
  for ( const auto &fileEntry : currentContainer.files() )
  {
    switch ( fileEntry->fileType() )
    {
      using enum Media::FileType;

      case RegularFile:
        regularFile( fileEntry, currentContainerElement );
        break;

      case LoadFile:
        load( fileEntry, currentContainerElement );
        break;

      case BatchFile:
        batch( fileEntry, currentContainerElement );
        break;

      default:
        // should never ever happen
        BOOST_THROW_EXCEPTION( Arinc665Exception()
          << Helper::AdditionalInfo{ "Invalid file type" } );
    }
  }
}

void Arinc665XmlSaveImpl::regularFile( const Media::ConstFilePtr &file, xmlpp::Element &parentElement ) const
{
  xmlpp::Element * const fileElement{ parentElement.add_child_element( "File" ) };
  assert( nullptr != fileElement );
  baseFile( file, *fileElement );
}

void Arinc665XmlSaveImpl::load( const Media::ConstFilePtr &file, xmlpp::Element &parentElement ) const
{
  xmlpp::Element * const loadElement{ parentElement.add_child_element( "Load" ) };
  assert( nullptr != loadElement );
  baseFile( file, *loadElement );

  const auto load{ std::dynamic_pointer_cast< const Media::Load >( file ) };
  assert( load );

  loadElement->set_attribute( "PartNumber", std::string( load->partNumber() ) );

  loadElement->set_attribute( "PartFlags", ARINC_FORMAT_NAMESPACE::format( "0x{:04X}", load->partFlags() ) );

  // Optional Load Type (Description + Type Value)
  if ( const auto &loadType{ load->loadType() }; loadType )
  {
    const auto &[ description, id ]{ *loadType };
    loadElement->set_attribute( "Description", description );
    loadElement->set_attribute( "Type", ARINC_FORMAT_NAMESPACE::format( "0x{:04X}", id ) );
  }

  // Load Check Value
  checkValue( *loadElement, "LoadCheckValue", load->loadCheckValueType() );

  // Data Files Check Value
  checkValue( *loadElement, "DataFilesCheckValue", load->dataFilesCheckValueType() );

  // Support Files Check Value
  checkValue( *loadElement, "SupportFilesCheckValue", load->supportFilesCheckValueType() );

  // iterate over target hardware
  for ( const auto &[targetHardwareId, positions] : load->targetHardwareIdPositions() )
  {
    auto * const targetHardwareElement{ loadElement->add_child_element( "TargetHardware" ) };
    targetHardwareElement->set_attribute( "ThwId", targetHardwareId );

    for( const auto &position : positions )
    {
      auto * const positionElement{ targetHardwareElement->add_child_element( "Position" ) };
      positionElement->set_attribute( "Pos", position );
    }
  }

  // data files
  loadFiles( load->dataFiles(), "DataFile", *loadElement );

  // support files
  loadFiles( load->supportFiles(), "SupportFile", *loadElement );

  if ( const auto &loadUserDefinedData{ load->userDefinedData() }; !loadUserDefinedData.empty() )
  {
    loadElement->add_child_element( "UserDefinedData" )->add_child_text( userDefinedData( loadUserDefinedData ) );
  }
}

void Arinc665XmlSaveImpl::loadFiles(
  const Media::ConstLoadFiles &files,
  std::string_view fileElementName,
  xmlpp::Element &loadElement ) const
{
  // iterate over files
  for ( const auto &[ file, partNumber, checkValueType ] : files )
  {
    auto * const fileElement{ loadElement.add_child_element( std::string{ fileElementName } ) };
    fileElement->set_attribute( "FilePath", file->path().string() );
    fileElement->set_attribute( "PartNumber", partNumber );

    if ( checkValueType )
    {
      fileElement->set_attribute(
        "CheckValue",
        std::string{ Arinc649::CheckValueTypeDescription::instance().name( *checkValueType ) } );
    }
  }
}

void Arinc665XmlSaveImpl::batch( const Media::ConstFilePtr &file, xmlpp::Element &parentElement ) const
{
  xmlpp::Element * const batchElement{ parentElement.add_child_element( "Batch" ) };
  assert( nullptr != batchElement );
  baseFile( file, *batchElement );

  const auto batch{ std::dynamic_pointer_cast< const Media::Batch >( file ) };
  assert( batch );

  batchElement->set_attribute( "PartNumber", std::string( batch->partNumber() ) );

  // set optional comment
  if ( const auto comment{ batch->comment() }; !comment.empty() )
  {
    batchElement->set_attribute( "Comment", std::string( comment ) );
  }

  // Iterate over batch information
  for ( const auto &[ thwIdPos, loads ] : batch->targets() )
  {
    auto * const targetElement{ batchElement->add_child_element( "Target" ) };

    targetElement->set_attribute( "ThwIdPos", thwIdPos );

    // iterate over loads
    for ( const auto &load : loads )
    {
      auto * const loadNode{ targetElement->add_child_element( "Load" ) };

      loadNode->set_attribute( "FilePath", load->path().string() );
    }
  }
}

void Arinc665XmlSaveImpl::baseFile( const Media::ConstFilePtr &file, xmlpp::Element &fileElement ) const
{
  // Add name attribute
  fileElement.set_attribute( "Name", std::string{ file->name() } );

  // Check Value Type
  if ( const auto checkValue{ file->checkValueType() }; checkValue )
  {
    fileElement.set_attribute(
      "CheckValue",
      std::string{ Arinc649::CheckValueTypeDescription::instance().name( *checkValue ) } );
  }

  // Add source path attribute (optional)
  if ( auto filePathIt{ filePathMappingV.find( file ) }; filePathIt != filePathMappingV.end() )
  {
    fileElement.set_attribute( "SourcePath", filePathIt->second.string() );
  }

  // Add medium if provided
  if ( const auto mediumNumber{ file->mediumNumber() }; mediumNumber )
  {
    fileElement.set_attribute( "Medium", std::to_string( static_cast< uint8_t >( *mediumNumber ) ) );
  }
}

void Arinc665XmlSaveImpl::checkValue(
  xmlpp::Element &element,
  std::string_view attribute,
  std::optional< Arinc649::CheckValueType > checkValue ) const
{
  if ( checkValue )
  {
    element.set_attribute(
      std::string{ attribute },
      std::string{ Arinc649::CheckValueTypeDescription::instance().name( *checkValue ) } );
  }
}

}
