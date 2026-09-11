# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_package( Qt6 COMPONENTS Widgets QUIET )
find_package( spdlog REQUIRED )

if( NOT TARGET Qt::Widgets )
  return()
endif()

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

add_library( arinc_665_qt )

include( GenerateExportHeader )
generate_export_header( arinc_665_qt )

target_sources(
  arinc_665_qt

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Arinc665Qt.hpp
        ExportMediaSetSettingsWidget.hpp
        FileCreationPolicyModel.hpp
        FilePathMappingModel.hpp
        ImportMediaSetXmlSettingsWidget.hpp
        MediaPathsModel.hpp
        MediaPathsWidget.hpp
        SupportedArinc665VersionModel.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/arinc_665_qt_export.h

  PRIVATE
    Arinc665Qt.cpp
    ExportMediaSetSettingsWidget.cpp
    ExportMediaSetSettingsWidget.ui
    FileCreationPolicyModel.cpp
    FilePathMappingModel.cpp
    ImportMediaSetXmlSettingsWidget.cpp
    ImportMediaSetXmlSettingsWidget.ui
    MediaPathsModel.cpp
    MediaPathsWidget.cpp
    MediaPathsWidget.ui
    SupportedArinc665VersionModel.cpp )

target_compile_features( arinc_665_qt PUBLIC cxx_std_23 )

target_compile_options(
  arinc_665_qt

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
  arinc_665_qt

  PUBLIC
    arinc_665
    Qt::Widgets

  PRIVATE
    spdlog::spdlog_header_only
    helper_qt
    qt_icon_resources
    arinc_649_qt
    helper )

add_subdirectory( actions )
add_subdirectory( compile_media_set )
add_subdirectory( decompile_media_set )
add_subdirectory( media )
add_subdirectory( media_set_manager )
add_subdirectory( media_set_viewer )
add_subdirectory( resources )

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
    ${CMAKE_CURRENT_BINARY_DIR}/arinc_665_qt_export.h )


install(
  TARGETS arinc_665_qt
  RUNTIME_DEPENDENCY_SET arinc_665-runtime-deps
  EXPORT arinc_665_qt-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT arinc_665_qt-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
