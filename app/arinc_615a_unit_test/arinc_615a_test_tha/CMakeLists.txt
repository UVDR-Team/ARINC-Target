# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

option(
  ARINC615A_THA_BUILD_TEST_APPLICATION
  "Build the standalone ARINC 615A THA test application (not suitable for a DKM)"
  OFF )

# This is the target-side implementation consumed by the C wrapper/DKM.  It
# has no process entry point and can therefore be linked into a C based DKM.
add_library( arinc_615a_tha_target STATIC )

target_sources(
  arinc_615a_tha_target

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
    Arinc615aTha.hpp
    DownloadOperationConfiguration.hpp
    InformationOperationConfiguration.hpp
    OperationConfiguration.hpp
    TargetDataLoaderConfiguration.hpp
    TargetInformationOperation.hpp
    TargetMediaDefinedDownloadOperation.hpp
    TargetOperation.hpp
    TargetOperatorDefinedDownloadOperation.hpp
    TargetUploadOperation.hpp
    UploadOperationConfiguration.hpp

  PRIVATE
    arinc_615a_test_tha.cpp
    DownloadOperationConfiguration.cpp
    InformationOperationConfiguration.cpp
    OperationConfiguration.cpp
    TargetDataLoaderConfiguration.cpp
    TargetInformationOperation.cpp
    TargetMediaDefinedDownloadOperation.cpp
    TargetOperatorDefinedDownloadOperation.cpp
    TargetUploadOperation.cpp
    UploadOperationConfiguration.cpp )

# The protocol source uses std::to_underlying and therefore requires C++23.
# Configure the Workbench DKM with the matching VxWorks C++23 runtime/ABI.
target_compile_features( arinc_615a_tha_target PUBLIC cxx_std_23 )

target_compile_options(
  arinc_615a_tha_target

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>
    # Disable Warning for exporting classes with std::* private members
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
    # Disable Warning for exporting classes, which derives from std::*
    $<$<CXX_COMPILER_ID:MSVC>:/wd4275>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  arinc_615a_tha_target

  PRIVATE
    arinc_615a
    arinc_665
    arinc_645 )

if( ARINC615A_THA_BUILD_TEST_APPLICATION )
  add_executable( arinc_615a_test_tha main.cpp )

  target_compile_features( arinc_615a_test_tha PRIVATE cxx_std_23 )

  target_link_libraries(
    arinc_615a_test_tha

    PRIVATE
      arinc_615a_tha_target
      # Windows Socket must be explicitly linked in MinGW
      $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

  configure_file(
    arinc_615a_test_tha.json.template
    arinc_615a_test_tha.json
    COPYONLY )
endif()

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

install(
  TARGETS arinc_615a_tha_target
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

if( ARINC615A_THA_BUILD_TEST_APPLICATION )
  install(
    TARGETS arinc_615a_test_tha
    COMPONENT test )

  install(
    FILES arinc_615a_test_tha.json.template
    TYPE BIN
    COMPONENT test )
endif()
