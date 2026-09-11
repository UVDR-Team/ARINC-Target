# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_package( Boost REQUIRED )

find_package( Qt6 COMPONENTS Core Svg QUIET )

if( NOT TARGET Qt::Core )
  return()
endif()

set( CMAKE_AUTOMOC ON )
#set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

# prepare Version.cpp
configure_file( Version.hpp.in Version.hpp )

add_library( qt_icon_resources )

include( GenerateExportHeader )
generate_export_header( qt_icon_resources )

target_sources(
  qt_icon_resources

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        QtIconResources.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/qt_icon_resources_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp

  PRIVATE
    QtIconResources.cpp
    font_awesome/font_awesome.qrc )

target_compile_features( qt_icon_resources PUBLIC cxx_std_23 )

target_compile_options(
  qt_icon_resources

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
  qt_icon_resources

  PUBLIC
    Qt::Core
    Qt::Svg
    helper )

# disable clang tidy in binary dir by placing dummy .clang-tidy file
file(
  WRITE
  ${CMAKE_CURRENT_BINARY_DIR}/.clang-tidy
  "Checks: '-*,llvm-twine-local'" )

add_library( qt_icon_resources_test OBJECT )

target_sources(
  qt_icon_resources_test

  PRIVATE
    test/VersionTest.cpp )

target_compile_features( qt_icon_resources_test PUBLIC cxx_std_23 )

target_compile_options(
  qt_icon_resources_test

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic>
    # Disable new Clang-22 Warning for usage of c2y-extensions (__COUNTER__) within boost::test
    $<$<CXX_COMPILER_ID:Clang>:-Wno-c2y-extensions> )

target_link_libraries(
  qt_icon_resources_test
  PUBLIC
    qt_icon_resources
    Boost::boost )


set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/qt_icon_resources_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp )

install(
  TARGETS qt_icon_resources
  RUNTIME_DEPENDENCY_SET qt_icon_resources-runtime-deps
  EXPORT qt_icon_resources-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT qt_icon_resources-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
