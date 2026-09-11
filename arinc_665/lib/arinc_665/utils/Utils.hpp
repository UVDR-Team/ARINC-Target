// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of namespace Arinc665::Utils.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc665::Utils.
 **/

#ifndef ARINC_665_UTILS_UTILS_HPP
#define ARINC_665_UTILS_UTILS_HPP

#include <arinc_665/Arinc665.hpp>

#include <arinc_665/media/Media.hpp>

#include <arinc_665/files/Files.hpp>

#include <arinc_665/MediumNumber.hpp>

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <tuple>

/**
 * @brief ARINC 665 Utilities.
 *
 * This namespace provides utilities for handling ARINC 665 %Media Sets:
 *  - Decompiling and Compiling %Media Sets
 *  - XML Handling
 *  - %Media Set Manager
 **/
namespace Arinc665::Utils {

//! %Media Paths (Medium Number -> Medium Path)
using MediaPaths = std::map< MediumNumber, std::filesystem::path >;

//! %Media Set Paths (%Media Set Base Path + %Media Paths)
using MediaSetPaths = std::pair< std::filesystem::path, MediaPaths >;

//! %Media Set Information (%Media Set and Check Values)
using MediaSetInformation = std::pair< Media::ConstMediaSetPtr, Media::CheckValues >;

//! %Media Sets Information (Part Number -> %Media Set Information)
using MediaSetsInformation = std::map< std::string, MediaSetInformation, std::less<> >;

/**
 * @name Media Set Compiler
 *
 * @{
 **/

//! %File creation policy of the compiler for %Load Headers/ %Batch %Files.
enum class FileCreationPolicy
{
  //! The compiler itself creates no file (load header/ batch file).
  None,
  //! The compiler itself creates only non-existing files.
  NoneExisting,
  //! All files are created by the compiler itself - even if already existing in the source.
  All
};

//! %File (Source) Path Mapping (File to Path)
using FilePathMapping = std::map< Media::ConstFilePtr, std::filesystem::path >;

class MediaSetCompiler;
//! ARINC 665 %Media Set Compiler Instance.
using MediaSetCompilerPtr = std::unique_ptr< MediaSetCompiler >;

class FilesystemMediaSetCompiler;
//! Filesystem ARINC 665 %Media Set Compiler Instance.
using FilesystemMediaSetCompilerPtr = std::unique_ptr< FilesystemMediaSetCompiler >;

/** @} **/

/**
 * @name Media Set Decompiler
 *
 * @{
 **/

//! %Media Set Decompiler Result Type
using MediaSetDecompilerResult = std::pair< Media::MediaSetPtr, Media::CheckValues >;

class MediaSetDecompiler;
//! ARINC 665 %Media Set Decompiler Instance.
using MediaSetDecompilerPtr = std::unique_ptr< MediaSetDecompiler >;

class FilesystemMediaSetDecompiler;
//! Filesystem ARINC 665 %Media Set Decompiler Instance.
using FilesystemMediaSetDecompilerPtr = std::unique_ptr< FilesystemMediaSetDecompiler >;

/** @} **/

/**
 * @name Load Decompiler
 *
 * @{
 **/

//! %Load Decompiler Result Type
using LoadDecompilerResult = std::tuple< Media::LoadPtr, Media::MediaSetPtr, Media::CheckValues >;

class LoadDecompiler;
class FilesystemLoadDecompiler;

//! ARINC 665 %Load Decompiler Instance Pointer.
using LoadDecompilerPtr = std::shared_ptr< LoadDecompiler >;

//! Filesystem ARINC 665 %Load Decompiler Instance.
using FilesystemLoadDecompilerPtr = std::unique_ptr< FilesystemLoadDecompiler >;

/** @}**/

class FilesystemMediaSetCopier;
//! Filesystem ARINC 665 %Media Set Copier Instance.
using FilesystemMediaSetCopierPtr = std::unique_ptr< FilesystemMediaSetCopier >;

class FilesystemMediaSetRemover;
//! Filesystem ARINC 665 %Media Set Remover Instance.
using FilesystemMediaSetRemoverPtr = std::unique_ptr< FilesystemMediaSetRemover >;

/**
 * @name Media Set Validator
 *
 * @{
 **/

class MediaSetValidator;
//! ARINC 665 %Media Set Validator Instance.
using MediaSetValidatorPtr = std::unique_ptr< MediaSetValidator >;

/** @} **/

/**
 * @name Media Set Manager
 *
 * @{
 **/

class MediaSetDefaults;
class MediaSetManagerConfiguration;
class MediaSetManager;
//! ARINC 665 %Media Set Manager Instance Pointer.
using MediaSetManagerPtr = std::shared_ptr< MediaSetManager >;

/** @} **/

/**
 * @brief Tries to decode the given directory as ARINC 665 Medium.
 *
 * Tries to access the *List of %Files* within the @p directory and decodes it.
 * If the decoding is successful, the medium information is returned.
 *
 * @param[in] directory
 *   Medium directory
 *
 * @return Medium Information for @p directory.
 * @retval {}
 *   If the @p directory is not a valid directory or ARINC 665 Medium
 **/
[[nodiscard]] ARINC_665_EXPORT std::optional< Files::MediaSetInformation > getMediumInformation(
  const std::filesystem::path &directory );

}

#endif
