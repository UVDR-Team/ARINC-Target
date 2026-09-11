# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

add_executable( arinc_665_media_set_manager )

target_sources(
  arinc_665_media_set_manager

  PRIVATE
    arinc_665_media_set_manager.cpp )

target_compile_features( arinc_665_media_set_manager PRIVATE cxx_std_23 )

target_compile_options(
  arinc_665_media_set_manager

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  arinc_665_media_set_manager

  PRIVATE
    arinc_665_commands
    commands
    helper )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY MAN_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_665_media_set_manager.adoc )

install(
  TARGETS arinc_665_media_set_manager
  RUNTIME_DEPENDENCY_SET arinc_665-runtime-deps
  COMPONENT runtime )
