# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

target_sources(
  arinc_665

  PUBLIC
    FILE_SET HEADERS
      FILES
        Arinc665File.hpp
        BatchFile.hpp
        BatchInfo.hpp
        BatchListFile.hpp
        BatchLoadInfo.hpp
        BatchTargetInfo.hpp
        CheckValueUtils.hpp
        FileInfo.hpp
        FileListFile.hpp
        Files.hpp
        ListFile.hpp
        LoadFileInfo.hpp
        LoadHeaderFile.hpp
        LoadInfo.hpp
        LoadListFile.hpp
        MediaSetInformation.hpp
        StringUtils.hpp

  PRIVATE
    Arinc665File.cpp
    BatchFile.cpp
    BatchInfo.cpp
    BatchListFile.cpp
    BatchLoadInfo.cpp
    BatchTargetInfo.cpp
    CheckValueUtils.cpp
    FileInfo.cpp
    FileListFile.cpp
    ListFile.cpp
    LoadFileInfo.cpp
    LoadHeaderFile.cpp
    LoadInfo.cpp
    LoadListFile.cpp
    StringUtils.cpp )

target_sources(
  arinc_665_test

  PRIVATE
    test/BatchFileTest.cpp
    test/BatchListFileTest.cpp
    test/BatchLoadInfoTest.cpp
    test/CheckValueUtilsTest.cpp
    test/FileListFileTest.cpp
    test/LoadFileInfoTest.cpp
    test/LoadHeaderFileTest.cpp
    test/LoadListFileTest.cpp
    test/StringUtilsTest.cpp )
