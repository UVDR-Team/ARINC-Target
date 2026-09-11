# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

target_sources(
  arinc_665

  PUBLIC
    FILE_SET HEADERS
      FILES
        Arinc665Xml.hpp
        FileCreationPolicyDescription.hpp
        FilePrinter.hpp
        FilesystemLoadDecompiler.hpp
        FilesystemMediaSetCompiler.hpp
        FilesystemMediaSetCopier.hpp
        FilesystemMediaSetDecompiler.hpp
        FilesystemMediaSetRemover.hpp
        LoadDecompiler.hpp
        MediaSetCompiler.hpp
        MediaSetDecompiler.hpp
        MediaSetDefaults.hpp
        MediaSetManager.hpp
        MediaSetManagerConfiguration.hpp
        MediaSetPrinter.hpp
        MediaSetValidator.hpp
        Utils.hpp

  PRIVATE
    Arinc665Xml.cpp
    FileCreationPolicyDescription.cpp
    FilePrinter.cpp
    FilesystemLoadDecompiler.cpp
    FilesystemMediaSetCompiler.cpp
    FilesystemMediaSetCopier.cpp
    FilesystemMediaSetDecompiler.cpp
    FilesystemMediaSetRemover.cpp
    LoadDecompiler.cpp
    MediaSetCompiler.cpp
    MediaSetDecompiler.cpp
    MediaSetDefaults.cpp
    MediaSetManager.cpp
    MediaSetManagerConfiguration.cpp
    MediaSetPrinter.cpp
    MediaSetValidator.cpp
    Utils.cpp )

set_source_files_properties(
  Arinc665Xml.cpp
  TARGET_DIRECTORY arinc_665
  PROPERTIES
    COMPILE_DEFINITIONS
    LIBXMLPPVERSION=${LIBXMLPPVERSION} )

#target_sources(
#  arinc_665_test
#
#  PRIVATE)

add_subdirectory( implementation )
