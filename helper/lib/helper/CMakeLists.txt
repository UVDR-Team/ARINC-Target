# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

if( ARINC_615A_VXWORKS )
  find_package( Boost REQUIRED )
  include_directories( BEFORE ${CMAKE_SOURCE_DIR}/compat )
else()
  find_package( Boost REQUIRED COMPONENTS program_options )
  find_package( spdlog REQUIRED )
endif()

# prepare Version.cpp
configure_file( Version.hpp.in Version.hpp )

add_library( helper )

include( GenerateExportHeader )
generate_export_header( helper )

target_sources(
  helper

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Bcd.hpp
        Bcd.ipp
        Bool.hpp
        Bool.ipp
        BoostAsioProgramOptions.hpp
        Description.hpp
        Description.ipp
        DescriptionEntry.hpp
        DescriptionEntry.ipp
        Dump.hpp
        EndianDescription.hpp
        Exception.hpp
        Filenames.hpp
        Helper.hpp
        KnownDirectories.hpp
        NBitInteger.hpp
        NBitInteger.ipp
        Parity.hpp
        Parity.ipp
        RawData.hpp
        RawData.ipp
        SafeCast.hpp
        SafeCast.ipp
        Scheduler.hpp
        SeverityLevelDescription.hpp
        VersionInformation.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/helper_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp

  PRIVATE
    BoostAsioProgramOptions.cpp
    Dump.cpp
    EndianDescription.cpp
    Filenames.cpp
    Helper.cpp
    Scheduler.cpp
    SeverityLevelDescription.cpp
    $<$<PLATFORM_ID:Linux>:implementation/KnownDirectoriesLinux.cpp>
    $<$<PLATFORM_ID:VxWorks>:implementation/KnownDirectoriesLinux.cpp>
    $<$<PLATFORM_ID:Windows>:implementation/KnownDirectoriesWindows.cpp> )

if( ARINC_615A_VXWORKS )
  target_compile_features( helper PUBLIC cxx_std_17 )
else()
  target_compile_features( helper PUBLIC cxx_std_23 )
endif()

target_compile_definitions(
  helper

  PUBLIC
    BOOST_ASIO_NO_DEPRECATED
    $<$<CXX_COMPILER_ID:MSVC>:BOOST_USE_WINAPI_VERSION=0x0A00>
    $<$<CXX_COMPILER_ID:MSVC>:_WIN32_WINNT=0x0A00> )

target_compile_options(
  helper

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  helper

  PUBLIC
    $<$<NOT:$<BOOL:${ARINC_615A_VXWORKS}>>:spdlog::spdlog_header_only>
    $<$<NOT:$<BOOL:${ARINC_615A_VXWORKS}>>:Boost::program_options>

  PRIVATE
    $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    $<$<PLATFORM_ID:Windows>:ws2_32> )

if( NOT ARINC_615A_VXWORKS )
  add_library( helper_test OBJECT )

  target_sources(
    helper_test

    PRIVATE
      test/BcdTest.cpp
      test/BoolTest.cpp
      test/DescriptionTest.cpp
      test/EndianDescriptionTest.cpp
      test/FilenamesTest.cpp
      test/KnownDirectoriesTest.cpp
      test/NBitIntegerTest.cpp
      test/ParityTest.cpp
      test/RawDataTest.cpp
      test/RawData_getInt1Test.cpp
      test/RawData_getInt2Test.cpp
      test/RawDataTest_setInt1Test.cpp
      test/RawDataTest_setInt2Test.cpp
      test/RawDataTest_toRaw1Test.cpp
      test/RawDataTest_toRaw2Test.cpp
      test/SafeCastTest.cpp
      test/SchedulerTest.cpp
      test/VersionTest.cpp )

  target_compile_features( helper_test PUBLIC cxx_std_23 )
  target_link_libraries( helper_test PUBLIC helper )
endif()

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/helper_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp )

install(
  TARGETS helper
  RUNTIME_DEPENDENCY_SET helper-runtime-deps
  EXPORT helper-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT helper-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
