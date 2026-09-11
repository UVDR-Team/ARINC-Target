# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

target_sources(
  arinc_665

  PRIVATE
    Arinc665XmlLoadImpl.hpp
    Arinc665XmlLoadImpl.cpp
    Arinc665XmlSaveImpl.hpp
    Arinc665XmlSaveImpl.cpp
    FilesystemLoadDecompilerImpl.hpp
    FilesystemLoadDecompilerImpl.cpp
    FilesystemMediaSetCompilerImpl.hpp
    FilesystemMediaSetCompilerImpl.cpp
    FilesystemMediaSetCopierImpl.hpp
    FilesystemMediaSetCopierImpl.cpp
    FilesystemMediaSetDecompilerImpl.hpp
    FilesystemMediaSetDecompilerImpl.cpp
    FilesystemMediaSetRemoverImpl.hpp
    FilesystemMediaSetRemoverImpl.cpp
    LoadDecompilerImpl.hpp
    LoadDecompilerImpl.cpp
    MediaSetCompilerImpl.hpp
    MediaSetCompilerImpl.cpp
    MediaSetDecompilerImpl.hpp
    MediaSetDecompilerImpl.cpp
    MediaSetManagerImpl.hpp
    MediaSetManagerImpl.cpp
    MediaSetValidatorImpl.cpp
    MediaSetValidatorImpl.hpp )

#target_sources(
#  arinc_665_test
#
#  PRIVATE)
