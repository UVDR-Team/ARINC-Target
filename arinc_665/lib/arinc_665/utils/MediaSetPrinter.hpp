// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Module Arinc665::Utils Media Set Printer.
 *
 * This module provides operations to print the content of ARINC 665 Media Sets.
 **/

#ifndef ARINC_665_UTILS_MEDIASETPRINTER_HPP
#define ARINC_665_UTILS_MEDIASETPRINTER_HPP

#include <arinc_665/utils/Utils.hpp>

#include <arinc_665/media/Media.hpp>

#include <iostream>
#include <string_view>

namespace Arinc665::Utils {

/**
 * @name ARINC 665 %Media Set Printer
 *
 * Can be used to dump the content of a %Media Set to the console.
 *
 * @{
 **/

/**
 * @brief Decodes and prints the Content of the %Media Set.
 *
 * - Prints files
 * - Prints loads
 * - Prints batches
 *
 * @param[in] mediaSet
 *   Media Set to print.
 * @param[in,out] outS
 *   Output Stream
 * @param[in] initialIndent
 *   Initial Indention prepended before each output.
 * @param[in] indent
 *   Indent for sub-information
 **/
ARINC_665_EXPORT void MediaSetPrinter_print(
  const Media::MediaSet &mediaSet,
  std::ostream &outS = std::cout,
  std::string_view initialIndent = {},
  std::string_view indent = " " );

/**
 * @brief Decodes and prints the content of the %Media Set File.
 *
 * - File Name
 * - File Path
 *
 * @param[in] file
 *   File to print.
 * @param[in,out] outS
 *   Output Stream
 * @param[in] initialIndent
 *   Initial Indention prepended before each output.
 **/
ARINC_665_EXPORT void MediaSetPrinter_print(
  const Media::File &file,
  std::ostream &outS = std::cout,
  std::string_view initialIndent = {} );

/**
 * @brief Decodes and prints the content of the Load.
 *
 * - Load Name
 * - Load Part Number
 * - Data files
 * - Support files
 *
 * @param[in] load
 *   Load to print.
 * @param[in,out] outS
 *   Output Stream
 * @param[in] initialIndent
 *   Initial Indention prepended before each output.
 * @param[in] indent
 *   Indent for sub-information
 **/
ARINC_665_EXPORT void MediaSetPrinter_print(
  const Media::Load &load,
  std::ostream &outS = std::cout,
  std::string_view initialIndent = {},
  std::string_view indent = " " );

/**
 * @brief Decodes and prints the content of the Batch.
 *
 * - Batch Name
 * - Batch Part Number
 * - Loads
 *
 * @param[in] batch
 *   Batch to print.
 * @param[in,out] outS
 *   Output Stream
 * @param[in] initialIndent
 *   Initial Indention prepended before each output.
 * @param[in] indent
 *   Indent for sub-information
 **/
ARINC_665_EXPORT void MediaSetPrinter_print(
  const Media::Batch &batch,
  std::ostream &outS = std::cout,
  std::string_view initialIndent = {},
  std::string_view indent = " " );

/**
 * @brief Decodes and prints the Check Values.
 *
 * - File Path
 * - Check Value
 *
 * @param[in] checkValues
 *   Check Values to print.
 * @param[in,out] outS
 *   Output Stream
 * @param[in] initialIndent
 *   Initial Indention prepended before each output.
 * @param[in] indent
 *   Indent for sub-information
 **/
ARINC_665_EXPORT void MediaSetPrinter_print(
  const Media::CheckValues &checkValues,
  std::ostream &outS = std::cout,
  std::string_view initialIndent = {},
  std::string_view indent = " " );

/** @} **/

}

#endif
