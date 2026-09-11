// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::Arinc665XmlSaveImpl.
 **/

#ifndef ARINC_665_UTILS_ARINC665XMLSAVEIMPL_HPP
#define ARINC_665_UTILS_ARINC665XMLSAVEIMPL_HPP

#include <arinc_665/utils/Utils.hpp>
#include <arinc_665/utils/Arinc665Xml.hpp>

#include <arinc_649/Arinc649.hpp>

#include <helper/RawData.hpp>

#include <libxml++/libxml++.h>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 Media Sets XML File Exporter.
 **/
class Arinc665XmlSaveImpl final
{
  public:
    /**
     * @brief Constructs the Media Set XML Exporter
     *
     * @param[in] mediaSet
     *   Media Set Information.
     * @param[in] filePathMapping
     *   File Path Mapping
     *   (used to insert the correct source path attribute.)
     * @param[in] xmlFile
     *   ARINC 665 XML file.
     **/
    Arinc665XmlSaveImpl(
      const Media::MediaSet &mediaSet,
      const FilePathMapping &filePathMapping,
      const std::filesystem::path &xmlFile );

    /**
     * @brief Saves the given Media Set information to the given XML file.
     *
     * @throw Arinc665Exception
     *   When XML file cannot be written.
     **/
    void operator()();

  private:

    /**
     * @brief Decodes User Defined data into string representation.
     *
     * @param[in] userDefinedData
     *   Decodes into string representation.
     *
     * @return User Defined Data String.
     **/
    [[nodiscard]] static std::string userDefinedData( Helper::ConstRawDataSpan userDefinedData );

    /**
     * @brief Export the Media Set section to the XML DOM.
     *
     * @param[in,out] mediaSetElement
     *   XML Element where to store media set information
     **/
    void mediaSet( xmlpp::Element &mediaSetElement ) const;

    /**
     * @brief Export container to XML DOM.
     *
     * Adds subdirectories and files.
     *
     * @param[in] currentContainer
     *   Current medium or directory.
     * @param[in,out] currentContainerElement
     *   XML Element, where to add content.
     **/
    void entries( const Media::ContainerEntity &currentContainer, xmlpp::Element &currentContainerElement ) const;

    /**
     * @brief Export Regular File to the XML DOM.
     *
     * @param[in] file
     *   File
     * @param[in,out] parentElement
     *   Parent XML Element
     **/
    void regularFile( const Media::ConstFilePtr &file, xmlpp::Element &parentElement ) const;

    /**
     * @brief Export Load to the XML DOM.
     *
     * @param[in] file
     *   Load.
     * @param[in,out] parentElement
     *   Parent XML Element
     **/
    void load( const Media::ConstFilePtr &file, xmlpp::Element &parentElement ) const;

    /**
     * @brief Export Load Files to the XML DOM.
     *
     * Load files are data or support files.
     *
     * @param[in] files
     *   Load Files
     * @param[in] fileElementName
     *   XML Element Name
     * @param[in,out] loadElement
     *   Parent XML Load Element
     **/
    void loadFiles(
      const Media::ConstLoadFiles &files,
      std::string_view fileElementName,
      xmlpp::Element &loadElement ) const;

    /**
     * @brief Export Batch to the XML DOM.
     *
     * @param[in] file
     *   Batch.
     * @param[in,out] parentElement
     *   Parent XML Element
     **/
    void batch( const Media::ConstFilePtr &file, xmlpp::Element &parentElement ) const;

    /**
     * @brief Export Base File Attributes to the XML DOM.
     *
     * Is called by regularFile(), lod(), and batch() to export common
     * attributes.
     * Stores:
     *  - `Name`,
     *  - `CheckValue`,
     *  - `SourcePath`, and
     *  - `Medium`.
     *
     * @param[in] file
     *   File
     * @param[in,out] fileElement
     *   XML Element
     **/
    void baseFile( const Media::ConstFilePtr &file, xmlpp::Element &fileElement ) const;

    /**
     * @brief Encodes the Check Value and stores it as attribute.
     *
     * @param[in,out] element
     *   XML Element
     * @param[in] attribute
     *   XML Attribute Name of Check Value
     * @param[in] checkValue
     *   CheckValue
     **/
    void checkValue(
      xmlpp::Element &element,
      std::string_view attribute,
      std::optional< Arinc649::CheckValueType > checkValue ) const;

    //! Media Set to export
    const Media::MediaSet &mediaSetV;
    //! File path mappings.
    const FilePathMapping &filePathMappingV;
    //! XML File Path
    const std::filesystem::path &xmlFileV;
    //! Deferred Load Loading Info
    std::list< std::tuple< const xmlpp::Element *, Media::LoadPtr > > deferredLoadInfoV;
    //! Deferred Batch Loading Info
    std::list< std::tuple< const xmlpp::Element *, Media::BatchPtr > > deferredBatchInfoV;
};

}

#endif
