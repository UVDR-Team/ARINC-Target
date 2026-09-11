// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::Arinc665XmlLoadImpl.
 **/

#include "Arinc665XmlLoadImpl.hpp"

#include <arinc_665/media/Batch.hpp>
#include <arinc_665/media/Directory.hpp>
#include <arinc_665/media/Load.hpp>
#include <arinc_665/media/MediaSet.hpp>
#include <arinc_665/media/RegularFile.hpp>

#include <arinc_665/Arinc665Exception.hpp>

#include <arinc_649/CheckValueTypeDescription.hpp>

#include <helper/Exception.hpp>
#include <helper/SafeCast.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Arinc665::Utils {

Arinc665XmlLoadImpl::Arinc665XmlLoadImpl( const std::filesystem::path &xmlFile ) :
  xmlFileV{ xmlFile }
{
}

LoadXmlResult Arinc665XmlLoadImpl::operator()()
{
  SPDLOG_INFO( "Load Media Set from '{}'", xmlFileV.string() );

  // Check the existence of the input XML file
  if ( !std::filesystem::is_regular_file( xmlFileV ) )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "XML File does not exist" }
      << boost::errinfo_file_name{ xmlFileV.string() } );
  }

  try
  {
    xmlpp::DomParser parser{};
    parser.parse_file( xmlFileV.string() );
    if ( !parser )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Error Parsing File" }
        << boost::errinfo_file_name{ xmlFileV.string() } );
    }

    auto * mediaSetElement{ parser.get_document()->get_root_node() };
    if ( ( nullptr == mediaSetElement ) || ( "MediaSet" != mediaSetElement->get_name() ) )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "MediaSet XML Element not found" }
        << boost::errinfo_file_name{ xmlFileV.string() } );
    }

    mediaSetV = Media::MediaSet::create();
    filePathMappingV.clear();
    deferredLoadInfoV.clear();
    deferredBatchInfoV.clear();

    mediaSet( *mediaSetElement );

    return std::make_tuple( std::move( mediaSetV ), std::move( filePathMappingV ) );
  }
  catch ( const xmlpp::exception &e )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ e.what() }
      << boost::errinfo_file_name{ xmlFileV.string() } );
  }
}

std::string Arinc665XmlLoadImpl::name( const xmlpp::Element &element )
{
  const auto name{ element.get_attribute_value( "Name" ) };

  if ( name.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "'Name' attribute missing or empty" }
      << boost::errinfo_at_line{ element.get_line() } );
  }

  return name;
}

OptionalMediumNumber Arinc665XmlLoadImpl::mediumNumber( const xmlpp::Element &element )
{
  if ( const auto medium{ element.get_attribute_value( "Medium" ) }; !medium.empty() )
  {
    const auto mediumValue{ std::stoull( medium ) };

    if ( !std::in_range< uint8_t >( mediumValue ) )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Invalid 'Medium' attribute value" }
        << boost::errinfo_at_line{ element.get_line() } );
    }

    return MediumNumber{ static_cast< uint8_t >( mediumValue ) };
  }

  return {};
}

Helper::RawData Arinc665XmlLoadImpl::userDefinedData( const std::string_view str )
{
  Helper::ConstRawDataSpan userDefinedDataSpan{ reinterpret_cast< std::byte const * >( str.data() ), str.size() };
  Helper::RawData userDefinedDataEncoded{ userDefinedDataSpan.begin(), userDefinedDataSpan.end() };
  if ( userDefinedDataEncoded.size() % 2 == 1 )
  {
    userDefinedDataEncoded.push_back( std::byte{ 0U } );
  }
  return userDefinedDataEncoded;
}

void Arinc665XmlLoadImpl::mediaSet( const xmlpp::Element &mediaSetElement )
{
  // Part Number
  const auto partNumber{ mediaSetElement.get_attribute_value( "PartNumber" ) };
  if ( partNumber.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "'PartNumber' attribute missing" }
      << boost::errinfo_at_line{ mediaSetElement.get_line() } );
  }
  mediaSetV->partNumber( partNumber );

  // Media Set Check Value
  if ( const auto mediaSetCheckValue{ checkValue( mediaSetElement, "MediaSetCheckValue" ) }; mediaSetCheckValue )
  {
    mediaSetV->mediaSetCheckValueType( mediaSetCheckValue );
  }

  // List of Files Check Value
  if (
    const auto listOfFilesCheckValue{ checkValue( mediaSetElement, "ListOfFilesCheckValue" ) };
    listOfFilesCheckValue )
  {
    mediaSetV->listOfFilesCheckValueType( listOfFilesCheckValue );
  }

  // List of Loads Check Value
  if (
    const auto listOfLoadsCheckValue{ checkValue( mediaSetElement, "ListOfLoadsCheckValue" ) };
    listOfLoadsCheckValue )
  {
    mediaSetV->listOfLoadsCheckValueType( listOfLoadsCheckValue );
  }

  // List of Batches Check Value
  if (
    const auto listOfBatchesCheckValue{ checkValue( mediaSetElement, "ListOfBatchesCheckValue" ) };
    listOfBatchesCheckValue )
  {
    mediaSetV->listOfBatchesCheckValueType( listOfBatchesCheckValue );
  }

  // Files Check Value
  if ( const auto filesCheckValue{ checkValue( mediaSetElement, "FilesCheckValue" ) }; filesCheckValue )
  {
    mediaSetV->filesCheckValueType( filesCheckValue );
  }

  if (
    auto const * const filesUserDefinedDataNode{
      dynamic_cast< const xmlpp::Element * >( mediaSetElement.get_first_child( "FilesUserDefinedData" ) ) };
    nullptr != filesUserDefinedDataNode )
  {
    mediaSetV->filesUserDefinedData(
      userDefinedData( filesUserDefinedDataNode->get_first_child_text()->get_content() ) );
  }

  if (
    auto const * const loadsUserDefinedDataNode{
      dynamic_cast< const xmlpp::Element * >( mediaSetElement.get_first_child( "LoadsUserDefinedData" ) ) };
    nullptr != loadsUserDefinedDataNode )
  {
    mediaSetV->loadsUserDefinedData(
      userDefinedData( loadsUserDefinedDataNode->get_first_child_text()->get_content() ) );
  }

  if (
    auto const * const batchesUserDefinedDataNode{ dynamic_cast< const xmlpp::Element*>(
      mediaSetElement.get_first_child( "BatchesUserDefinedData") ) };
    nullptr != batchesUserDefinedDataNode )
  {
    mediaSetV->batchesUserDefinedData(
      userDefinedData( batchesUserDefinedDataNode->get_first_child_text()->get_content() ) );
  }

  // content node
  const auto *contentNode{ dynamic_cast< const xmlpp::Element * >( mediaSetElement.get_first_child( "Content" ) ) };
  if ( nullptr == contentNode )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "No 'Content' element given" }
      << boost::errinfo_at_line{ mediaSetElement.get_line() } );
  }

  // load files
  entries( *contentNode, *mediaSetV );

  // deferred loading of loads and batches
  for ( const auto &[ loadElement, load ] : deferredLoadInfoV )
  {
    loadDeferred( *loadElement, *load );
  }
  for ( const auto &[ batchElement, batch ] : deferredBatchInfoV )
  {
    loadBatchDeferred( *batchElement, *batch );
  }
}

void Arinc665XmlLoadImpl::entries(
  const xmlpp::Element &currentContainerElement,
  Media::ContainerEntity &currentContainer )
{
  // Common Default Medium attribute for directories and Contents root
  if (
    const auto defaultMedium{ currentContainerElement.get_attribute_value( "DefaultMedium" ) };
    !defaultMedium.empty() )
  {
    const auto defaultMediumValue{ std::stoull( defaultMedium ) };

    if ( !std::in_range< uint8_t >( defaultMediumValue ) )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Invalid DefaultMedium value" }
        << boost::errinfo_at_line{ currentContainerElement.get_line() } );
    }

    currentContainer.defaultMediumNumber( MediumNumber{ static_cast< uint8_t >( defaultMediumValue ) } );
  }

  // iterate over all XML nodes
  for ( auto const * const entryNode : currentContainerElement.get_children() )
  {
    auto const * const entryElement{ dynamic_cast< xmlpp::Element const * >( entryNode ) };

    if ( nullptr == entryElement )
    {
       // skip non-elements
      continue;
    }

    switch ( entryType( *entryElement ) )
    {
      using enum EntryType;

      case Directory:
        // add subdirectory and add content recursively
        entries( *entryElement, *( currentContainer.addSubdirectory( name( *entryElement ) ) ) );
        break;

      case RegularFile:
        regularFile( *entryElement, currentContainer );
        break;

      case LoadFile:
        load( *entryElement, currentContainer );
        break;

      case BatchFile:
        batch( *entryElement, currentContainer );
        break;

      default:
        break;
    }
  }
}

void Arinc665XmlLoadImpl::regularFile( const xmlpp::Element &fileElement, Media::ContainerEntity &parent )
{
  auto file{ parent.addRegularFile( name( fileElement ), mediumNumber( fileElement ) ) };

  baseFile( fileElement, file );
}

void Arinc665XmlLoadImpl::load( const xmlpp::Element &loadElement, Media::ContainerEntity &parent )
{
  auto load{ parent.addLoad( name( loadElement ), mediumNumber( loadElement ) ) };

  baseFile( loadElement, load );

  // Part Number
  auto partNumber{ loadElement.get_attribute_value( "PartNumber" ) };
  if ( partNumber.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "PartNumber attribute missing or empty" }
      << boost::errinfo_at_line{ loadElement.get_line() } );
  }
  load->partNumber( std::move( partNumber ) );

  // Part Flags
  if ( const auto partFlags{ loadElement.get_attribute_value( "PartFlags" ) }; !partFlags.empty() )
  {
    const uint16_t partFlagsValue{ Helper::safeCast< uint16_t >( std::stoul( partFlags, nullptr, 0 ) ) };

    load->partFlags( partFlagsValue );
  }

  // Load Type (Description + Type Value)
  if (
    const auto &[ description, type ]{ std::make_tuple(
      loadElement.get_attribute_value( "Description" ),
      loadElement.get_attribute_value( "Type" ) ) };
    !type.empty() && !description.empty() )
  {
    const uint16_t typeValue{ Helper::safeCast< uint16_t >( std::stoul( type, nullptr, 0 ) ) };

    load->loadType( { std::make_pair( description, typeValue ) } );
  }

  Media::Load::TargetHardwareIdPositions thwIds{};

  // iterate over target hardware
  for ( auto const * const targetHardwareNode : loadElement.get_children( "TargetHardware" ) )
  {
    if ( nullptr == targetHardwareNode )
    {
      // Should never occur
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "TargetHardware Node is null" }
        << boost::errinfo_at_line{ loadElement.get_line() } );
    }

    const auto * const targetHardwareElement{ dynamic_cast< xmlpp::Element const * >( targetHardwareNode ) };
    if ( nullptr == targetHardwareElement )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "TargetHardware Element invalid" }
        << boost::errinfo_at_line{ targetHardwareNode->get_line() } );
    }

    auto thwId{ targetHardwareElement->get_attribute_value( "ThwId" ) };

    Media::Load::Positions positions{};

    // iterate over position XML elements
    for ( auto const * const positionNode : targetHardwareElement->get_children( "Position" ) )
    {
      if ( nullptr == positionNode )
      {
        // Should never occur
        BOOST_THROW_EXCEPTION( Arinc665Exception()
          << Helper::AdditionalInfo{ "Position Node is null" }
          << boost::errinfo_at_line{ targetHardwareElement->get_line() } );
      }

      const auto * const positionElement{ dynamic_cast< xmlpp::Element const * >( positionNode ) };
      if ( nullptr == positionElement )
      {
        BOOST_THROW_EXCEPTION( Arinc665Exception()
          << Helper::AdditionalInfo{ "Position Element invalid" }
          << boost::errinfo_at_line{ positionNode->get_line() } );
      }

      auto position{ positionElement->get_attribute_value( "Pos" ) };

      positions.emplace( position );
    }

    thwIds.try_emplace( thwId, std::move( positions ) );
  }

  load->targetHardwareIdPositions( std::move( thwIds ) );

  //  handle data and support files later in deferred loading

  if (
    const auto * const userDefinedDataElement{ dynamic_cast< const xmlpp::Element*>(
      loadElement.get_first_child( "UserDefinedData" ) ) };
    nullptr != userDefinedDataElement )
  {
    load->userDefinedData( userDefinedData( userDefinedDataElement->get_first_child_text()->get_content() ) );
  }

  // Load Check Value
  if ( const auto loadCheckValue{ checkValue( loadElement, "LoadCheckValue" ) }; loadCheckValue )
  {
    load->loadCheckValueType( loadCheckValue );
  }

  // Data Files Check Value (optional)
  if ( const auto dataFilesCheckValue{ checkValue( loadElement, "DataFilesCheckValue" ) }; dataFilesCheckValue )
  {
    load->dataFilesCheckValueType( dataFilesCheckValue );
  }

  // Support Files Check Value (optional)
  if (
    const auto supportFilesCheckValue{ checkValue( loadElement, "SupportFilesCheckValue" ) };
    supportFilesCheckValue )
  {
    load->supportFilesCheckValueType( supportFilesCheckValue );
  }

  // add load to deferred load list
  deferredLoadInfoV.emplace_back( &loadElement, load );
}

void Arinc665XmlLoadImpl::loadDeferred( const xmlpp::Element &loadElement, Media::Load &load )
{
  const auto &loadParent{ *load.parent() };

  // data files
  load.dataFiles( loadFiles( loadElement, "DataFile", loadParent ) );

  // support files
  load.supportFiles( loadFiles( loadElement, "SupportFile", loadParent ) );
}

Media::ConstLoadFiles Arinc665XmlLoadImpl::loadFiles(
  const xmlpp::Element &loadElement,
  const std::string_view fileElementsName,
  const Media::ContainerEntity &parent ) const
{
  Media::ConstLoadFiles loadFiles{};

  // iterate over file elements
  for ( auto const * const fileNode : loadElement.get_children( std::string{ fileElementsName } ) )
  {
    if ( nullptr == fileNode )
    {
      // Should never occur
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "File Node is null" }
        << boost::errinfo_at_line{ loadElement.get_line() } );
    }

    const auto * fileElement{ dynamic_cast< xmlpp::Element const * >( fileNode ) };
    if ( nullptr == fileElement )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "File Element invalid" }
        << boost::errinfo_at_line{ fileNode->get_line() } );
    }

    const auto filePath{ fileElement->get_attribute_value( "FilePath" ) };
    if ( filePath.empty() )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "'FilePath' attribute missing or empty" }
        << boost::errinfo_at_line{ fileNode->get_line() } );
    }

    const auto filePartNumber{ fileElement->get_attribute_value( "PartNumber" ) };
    if ( filePartNumber.empty() )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "'PartNumber' attribute missing or empty" }
        << boost::errinfo_at_line{ fileNode->get_line() } );
    }

    //  Check Value
    auto checkValueType{ checkValue( *fileElement, "CheckValue" ) };

    // Find File
    auto file{ parent.regularFile( std::filesystem::path{ filePath } ) };

    if ( !file )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "'FilePath' attribute does not reference file" }
        << boost::errinfo_at_line{ fileNode->get_line() } );
    }

    loadFiles.emplace_back( file, filePartNumber, checkValueType );
  }

  return loadFiles;
}

void Arinc665XmlLoadImpl::batch( const xmlpp::Element &batchElement, Media::ContainerEntity &parent )
{
  auto batch{ parent.addBatch( name( batchElement ), mediumNumber( batchElement ) ) };

  baseFile( batchElement, batch );

  auto partNumber{ batchElement.get_attribute_value( "PartNumber" ) };
  if ( partNumber.empty() )
  {
    BOOST_THROW_EXCEPTION( Arinc665Exception()
      << Helper::AdditionalInfo{ "PartNumber attribute missing or empty" }
      << boost::errinfo_at_line{ batchElement.get_line() } );
  }
  batch->partNumber( std::move( partNumber ) );

  auto comment{ batchElement.get_attribute_value( "Comment" ) };
  batch->comment( std::move( comment ) );

  // handle batch load file handling in deferred batch loading

  // add load to deferred load list
  deferredBatchInfoV.emplace_back( &batchElement, batch );
}

void Arinc665XmlLoadImpl::loadBatchDeferred( const xmlpp::Element &batchElement, Media::Batch &batch )
{
  // iterate over target - XML elements
  for ( auto const * const targetNode : batchElement.get_children( "Target" ) )
  {
    if ( nullptr == targetNode )
    {
      // Should never occur
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Target Node is null" }
        << boost::errinfo_at_line{ batchElement.get_line() } );
    }

    const auto * const targetElement{ dynamic_cast< xmlpp::Element const * >( targetNode ) };
    if ( nullptr == targetElement )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Invalid Target Element" }
        << boost::errinfo_at_line{ targetNode->get_line() } );
    }

    const auto thwIdPos{ targetElement->get_attribute_value( "ThwIdPos" ) };

    Media::ConstLoads targetLoads{};

    // iterate over loads
    for ( auto const * const loadNode : targetNode->get_children( "Load" ) )
    {
      if ( nullptr == loadNode )
      {
        // Should never occur
        BOOST_THROW_EXCEPTION( Arinc665Exception()
          << Helper::AdditionalInfo{ "Load Node is null" }
          << boost::errinfo_at_line{ targetNode->get_line() } );
      }

      auto const * const loadElement{ dynamic_cast< xmlpp::Element const * >( loadNode ) };
      if ( nullptr == loadElement)
      {
        BOOST_THROW_EXCEPTION( Arinc665Exception()
          << Helper::AdditionalInfo{ "Node is not an Element" }
          << boost::errinfo_at_line{ loadNode->get_line() } );
      }

      const auto loadFilePath{ loadElement->get_attribute_value( "FilePath" ) };
      if ( loadFilePath.empty() )
      {
        BOOST_THROW_EXCEPTION( Arinc665Exception()
          << Helper::AdditionalInfo{ "FilePath attribute missing or empty" }
          << boost::errinfo_at_line{ loadElement->get_line() } );
      }

      auto load{ batch.parent()->load( std::filesystem::path{ loadFilePath, std::locale{} } ) };
      if ( !load )
      {
        BOOST_THROW_EXCEPTION( Arinc665Exception()
          << Helper::AdditionalInfo{ "FilePath attribute does not reference load" }
          << boost::errinfo_at_line{ loadElement->get_line() }
          << boost::errinfo_file_name{ loadFilePath } );
      }

      targetLoads.push_back( load );
    }

    // add THW ID POS with Loads
    batch.target( thwIdPos, targetLoads );
  }
}

void Arinc665XmlLoadImpl::baseFile( const xmlpp::Element &fileElement, const Media::FilePtr &file )
{
  // File Check Value
  const auto fileCheckValue{ checkValue( fileElement, "CheckValue" ) };

  file->checkValueType( fileCheckValue );

  // common source path attribute for files - set the source path if attribute is present
  if ( const auto sourcePath{ fileElement.get_attribute_value( "SourcePath" ) }; !sourcePath.empty() )
  {
    filePathMappingV.try_emplace( file, sourcePath );
  }
}

std::optional< Arinc649::CheckValueType > Arinc665XmlLoadImpl::checkValue(
  const xmlpp::Element &element,
  const std::string_view attribute )
{
  if (
    const auto checkValueString{ element.get_attribute_value( std::string{ attribute } ) };
    !checkValueString.empty() )
  {
    const auto checkValue{
      Arinc649::CheckValueTypeDescription::instance().enumeration( static_cast< std::string >( checkValueString ) ) };

    if ( !checkValue )
    {
      BOOST_THROW_EXCEPTION( Arinc665Exception()
        << Helper::AdditionalInfo{ "Invalid Check Value" }
        << boost::errinfo_at_line{ element.get_line() } );
    }

    return checkValue;
  }

  return {};
}

Arinc665XmlLoadImpl::EntryType Arinc665XmlLoadImpl::entryType( const xmlpp::Element &element )
{
  using namespace std::string_literals;

  if ( element.get_name() == "Directory"s )
  {
    return EntryType::Directory;
  }

  if ( element.get_name() == "File"s )
  {
    return EntryType::RegularFile;
  }

  if ( element.get_name() == "Load"s )
  {
    return EntryType::LoadFile;
  }

  if ( element.get_name() == "Batch"s )
  {
    return EntryType::BatchFile;
  }

  BOOST_THROW_EXCEPTION( Arinc665Exception()
    << Helper::AdditionalInfo{ "Invalid XML Element" }
    << boost::errinfo_at_line{ element.get_line() }
    << boost::errinfo_type_info_name{ element.get_name() } );
}

}
