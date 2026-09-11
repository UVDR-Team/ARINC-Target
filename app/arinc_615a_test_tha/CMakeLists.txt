# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

# Build the core THA Target library (suitable for linking into VxWorks DKMs or test apps)
add_library( arinc_615a_test_tha_lib STATIC )

target_sources(
  arinc_615a_test_tha_lib

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
      FILES
        arinc_615a_tha.h
        Arinc615aThaLib.hpp
        TargetDataLoaderConfiguration.hpp
        OperationConfiguration.hpp
        InformationOperationConfiguration.hpp
        UploadOperationConfiguration.hpp
        DownloadOperationConfiguration.hpp
        TargetOperation.hpp

  PRIVATE
    Arinc615aThaLib.cpp
    Arinc615aTha.hpp
    DownloadOperationConfiguration.cpp
    InformationOperationConfiguration.cpp
    OperationConfiguration.cpp
    TargetDataLoaderConfiguration.cpp
    TargetInformationOperation.hpp
    TargetInformationOperation.cpp
    TargetMediaDefinedDownloadOperation.hpp
    TargetMediaDefinedDownloadOperation.cpp
    TargetOperatorDefinedDownloadOperation.hpp
    TargetOperatorDefinedDownloadOperation.cpp
    TargetUploadOperation.hpp
    TargetUploadOperation.cpp
    UploadOperationConfiguration.cpp )

target_include_directories(
  arinc_615a_test_tha_lib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/compat> )

target_compile_features( arinc_615a_test_tha_lib PUBLIC cxx_std_17 )

target_compile_options(
  arinc_615a_test_tha_lib

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4275>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  arinc_615a_test_tha_lib

  PUBLIC
    arinc_615a
    arinc_665
    arinc_649
  PRIVATE
    $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

# Standalone executable frontend (built for host testing, skipped for VxWorks DKM)
if( NOT ARINC_615A_VXWORKS )
  add_executable( arinc_615a_test_tha arinc_615a_test_tha.cpp )

  target_compile_features( arinc_615a_test_tha PRIVATE cxx_std_17 )

  target_link_libraries(
    arinc_615a_test_tha
    PRIVATE
      arinc_615a_test_tha_lib )

  configure_file(
    arinc_615a_test_tha.json.template
    arinc_615a_test_tha.json
    COPYONLY )

  install(
    TARGETS arinc_615a_test_tha
    RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps
    COMPONENT test )

  install(
    FILES arinc_615a_test_tha.json.template
    TYPE BIN
    COMPONENT test )
else()
  add_library( arinc_615a_test_tha ALIAS arinc_615a_test_tha_lib )
endif()

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )
