# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_package( Qt6 COMPONENTS Widgets QUIET )

if( NOT TARGET Qt::Widgets )
  return()
endif()

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
#set( CMAKE_AUTORCC ON )

add_library( helper_qt )

include( GenerateExportHeader )
generate_export_header( helper_qt )

target_sources(
  helper_qt

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        AboutDialog.hpp
        EndianModel.hpp
        HelperQt.hpp
        #QBoostLoggingModel.hpp
        String.hpp
        VersionsModel.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/helper_qt_export.h

  PRIVATE
    AboutDialog.cpp
    AboutDialog.ui
    EndianModel.cpp
    HelperQt.cpp
    #QBoostLoggingModel.cpp
    String.cpp
    VersionsModel.cpp )

target_compile_features( helper_qt PUBLIC cxx_std_23 )

target_compile_options(
  helper_qt

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
  helper_qt

  PUBLIC
    Qt::Widgets
    helper )

# disable clang tidy in binary dir by placing dummy .clang-tidy file
file(
  WRITE
  ${CMAKE_CURRENT_BINARY_DIR}/.clang-tidy
  "Checks: '-*,llvm-twine-local'" )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/helper_qt_export.h )


install(
  TARGETS helper_qt
  RUNTIME_DEPENDENCY_SET helper-runtime-deps
  EXPORT helper_qt-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT helper_qt-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
