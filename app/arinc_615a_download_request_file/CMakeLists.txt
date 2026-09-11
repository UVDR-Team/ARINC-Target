# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

add_executable( arinc_615a_download_request_file )

target_sources(
  arinc_615a_download_request_file
  PRIVATE
    arinc_615a_download_request_file.cpp )

target_compile_features( arinc_615a_download_request_file PRIVATE cxx_std_23 )

target_compile_options(
  arinc_615a_download_request_file

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

target_link_libraries( arinc_615a_download_request_file PRIVATE arinc_615a )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY MAN_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_download_request_file.adoc )

install(
  TARGETS arinc_615a_download_request_file
  RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps
  COMPONENT runtime )

# Copy Own DLLs on Windows and Shared build to App directory for direct
# execution.
# TODO $<TARGET_RUNTIME_DLLS:arinc_615a_download_request_file> doesn't
#      work - Boost defines wrong paths ('lib/' <-> 'bin'/)
if( WIN32 AND BUILD_SHARED_LIBS )
  add_custom_command(
    TARGET arinc_615a_download_request_file
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      -E copy
      -t
      $<TARGET_FILE_DIR:arinc_615a_download_request_file>
      #$<TARGET_RUNTIME_DLLS:arinc_615a_download_request_file>
      $<TARGET_FILE:helper>
      $<TARGET_FILE:arinc_649>
      $<TARGET_FILE:arinc_665>
      $<TARGET_FILE:tftp>
      $<TARGET_FILE:arinc_615a>
    COMMAND_EXPAND_LISTS )
endif()
