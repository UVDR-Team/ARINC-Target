# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

# skip app when arinc_665_qt is not available
if( NOT TARGET arinc_665_qt )
  return()
endif()

add_executable( arinc_665_media_set_manager_gui WIN32 )

target_sources(
  arinc_665_media_set_manager_gui

  PRIVATE
    arinc_665_media_set_manager_gui.cpp )

target_compile_features( arinc_665_media_set_manager_gui PRIVATE cxx_std_23 )

target_compile_options(
  arinc_665_media_set_manager_gui

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
  arinc_665_media_set_manager_gui

  PRIVATE
    arinc_665_qt
    arinc_665
    qt_icon_resources
    helper )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

install(
  TARGETS arinc_665_media_set_manager_gui
  RUNTIME_DEPENDENCY_SET arinc_665-runtime-deps
  COMPONENT runtime )
