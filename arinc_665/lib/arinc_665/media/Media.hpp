// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc665::Media.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc665::Media.
 **/

#ifndef ARINC_665_MEDIA_MEDIA_HPP
#define ARINC_665_MEDIA_MEDIA_HPP

#include <arinc_665/Arinc665.hpp>

#include <arinc_649/Arinc649.hpp>

#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <tuple>
#include <variant>

/**
 * @brief ARINC 665 %Media Set Model.
 *
 * This namespace provides classes to represent a ARINC 665 %Media Set and their parts like Directories, Loads, Batches,
 * and other %Files.
 **/
namespace Arinc665::Media {

//! Item Type
enum class Type
{
  //! %Media Set
  MediaSet,
  //! %Directory within %Media Set
  Directory,
  //! %File within %Media Set / %Directory (Refined by @ref FileType)
  File
};

//! File Type
enum class FileType
{
  //! Regular %File (can be part of a %Load)
  RegularFile,
  //! %Load Header %File
  LoadFile,
  //! %Batch %File
  BatchFile
};

/**
 * @name %Base Class Declarations
 * @{
 **/

class Base;
//! Constant %Base Pointer
using ConstBasePtr = std::shared_ptr< const Base >;
//! %Base Pointer
using BasePtr = std::shared_ptr< Base >;

/** @} **/

/**
 * @name Media Set Declarations
 * @{
 **/

class MediaSet;
//! Constant %Media Set Pointer
using ConstMediaSetPtr = std::shared_ptr< const MediaSet >;
//! %Media Set Pointer
using MediaSetPtr = std::shared_ptr< MediaSet >;
//! List of %Media Sets
using MediaSets = std::list< MediaSetPtr >;
//! List of Constant %Media Sets
using ConstMediaSets = std::list< ConstMediaSetPtr >;
//! %Media Set Variant
using MediaSetVariant = std::variant< MediaSetPtr, ConstMediaSetPtr >;
//! %Media Sets Variant
using MediaSetsVariant = std::variant< MediaSets, ConstMediaSets >;

/** @} **/

/**
 * @name Container Declarations
 * @{
 **/

class ContainerEntity;
//! Constant %Container Entity Pointer
using ConstContainerEntityPtr = std::shared_ptr< const ContainerEntity >;
//! %Container Entity Pointer
using ContainerEntityPtr = std::shared_ptr< ContainerEntity >;

/** @} **/

/**
 * @name Directory Declarations
 * @{
 **/

class Directory;

//! Constant %Directory Pointer
using ConstDirectoryPtr = std::shared_ptr< const Directory >;
//! %Directory Pointer
using DirectoryPtr = std::shared_ptr< Directory >;
//! Constant %Directories (List)
using ConstDirectories = std::list< ConstDirectoryPtr >;
//! %Directories (List)
using Directories = std::list< DirectoryPtr >;

/** @} **/

/**
 * @name File Declarations
 * @{
 **/
class File;

//! Pointer to Const %File
using ConstFilePtr = std::shared_ptr< const File >;
//! Pointer to %File
using FilePtr = std::shared_ptr< File >;
//! List of %Files
using Files = std::list< FilePtr >;
//! List of Constant %Files
using ConstFiles = std::list< ConstFilePtr >;

/** @} **/

/**
 * @name Regular File Declarations
 * @{
 **/
class RegularFile;

//! Pointer to Const Regular %File
using ConstRegularFilePtr = std::shared_ptr< const RegularFile > ;
//! Pointer to Regular %File
using RegularFilePtr = std::shared_ptr< RegularFile >;
//! Regular %Files (List)
using RegularFiles = std::list< RegularFilePtr >;
//! Constant Regular %Files (List)
using ConstRegularFiles = std::list< ConstRegularFilePtr >;

/** @} **/

/**
 * @name Load Declarations
 * @{
 **/
class Load;

//! Constant %Load Pointer
using ConstLoadPtr = std::shared_ptr< const Load >;
//! %Load Pointer
using LoadPtr = std::shared_ptr< Load >;
//! %Loads List
using Loads = std::list< LoadPtr >;
//! Constant %Loads List
using ConstLoads = std::list< ConstLoadPtr >;
//! %Load Variants
using LoadVariant = std::variant< LoadPtr, ConstLoadPtr >;
//! %Loads Variant
using LoadsVariant = std::variant< Loads, ConstLoads >;

//! Const %Load %File List (File, Part Number, Check Value Type for this File).
//! Used for Data and Support Files of Load
using ConstLoadFile = std::tuple< ConstRegularFilePtr, std::string, std::optional< Arinc649::CheckValueType > >;

//! Const %Load %File List. Used for Data and Support Files of Load.
using ConstLoadFiles = std::list< ConstLoadFile >;

/** @} **/

/**
 * @name Batch Declarations
 * @{
 **/
class Batch;

//! Constant %Batch Pointer.
using ConstBatchPtr = std::shared_ptr< const Batch > ;
//! %Batch Pointer.
using BatchPtr = std::shared_ptr< Batch > ;
//! %Batches List.
using Batches = std::list< BatchPtr >;
//! Constant %Batches List.
using ConstBatches = std::list< ConstBatchPtr >;
//! %Batch Variants.
using BatchVariant = std::variant< BatchPtr, ConstBatchPtr >;
//! %Batches Variant.
using BatchesVariant = std::variant< Batches, ConstBatches >;

//! %Batch Information (Maps: Target Hardware ID Position -> Loads)
using BatchInformation = std::map< std::string, Loads, std::less<> >;
//! Const %Batch Information (Maps: Target Hardware ID Position -> Loads)
using ConstBatchInformation = std::map< std::string, ConstLoads, std::less<> >;
//! %Batch Information Variant.
using BatchInformationVariant = std::variant< BatchInformation, ConstBatchInformation >;
//! %Batch Target Information.
using BatchTargetInformation = BatchInformation::value_type;
//! Const %Batch Target Information.
using ConstBatchTargetInformation = ConstBatchInformation::value_type;
//! %Batch Target Information Variant.
using BatchTargetInformationVariant = std::variant< BatchTargetInformation, ConstBatchTargetInformation >;

/** @} **/

/**
 * @name Directory / Files Declarations
 * @{
 **/

//! Tuple of directory and a const file
using ConstDirectoryFileTuple  = std::tuple< const Directory&, ConstFilePtr >;
//! Tuple of directory and a file
using DirectoryFileTuple = std::tuple< Directory&, FilePtr >;

/** @} **/

//! Check Values of %Media Set %Files.
using CheckValues = std::map< ConstFilePtr, std::set< Arinc649::CheckValue >, std::owner_less< ConstFilePtr > >;

}

#endif
