// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc665::Files.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of namespace Arinc665::Files.
 **/

#ifndef ARINC_665_FILES_FILES_HPP
#define ARINC_665_FILES_FILES_HPP

#include <arinc_665/Arinc665.hpp>

#include <list>

/**
 * @brief ARINC 665 %Media Set %Files.
 *
 * This namespace contains the implementation of the following ARINC 665 protocol files:
 * - List files:
 *   - List of %Files: @ref FileListFile
 *   - List of Loads: @ref LoadListFile
 *   - List of Batches: @ref BatchListFile
 * - Load Header File: @ref LoadHeaderFile
 * - Batch File: @ref BatchFile
 *
 * The implementation supports the file formats of the following ARINC 665
 * supplements:
 * - Supplement 2
 * - Supplement 3
 * - Supplement 4 (Which actually does not refine the file formats)
 * - Supplement 5 (Which actually does not refine the file formats)
 **/
namespace Arinc665::Files {

struct MediaSetInformation;

class Arinc665File;

/**
 * @name Load File Information
 *
 * @{
 **/

struct LoadFileInfo;
//! Load Files Information.
using LoadFilesInfo = std::list< LoadFileInfo >;
class LoadHeaderFile;

/** @} **/

/**
 * @name Batch File Information
 *
 * @{
 **/

struct BatchTargetInfo;
//! Batch Targets Information.
using BatchTargetsInfo = std::list< BatchTargetInfo >;
struct BatchLoadInfo;
//! Batch Loads Information.
using BatchLoadsInfo = std::list< BatchLoadInfo >;
class BatchFile;

/** @} **/

/**
 * @name batch List File Information
 *
 * @{
 **/

struct BatchInfo;
//! Batches Information.
using BatchesInfo = std::list< BatchInfo >;
class BatchListFile;

/** @} **/

/**
 * @name Load List File Information
 *
 * @{
 **/

struct LoadInfo;
//! Loads Information.
using LoadsInfo = std::list< LoadInfo >;
class LoadListFile;

/** @} **/

/**
 * @name File List File Information
 *
 * @{
 **/

struct FileInfo;
//! Files Information.
using FilesInfo = std::list< FileInfo >;
class FileListFile;

/** @} **/

}

#endif
