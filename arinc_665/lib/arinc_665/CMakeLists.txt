# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_package( Boost REQUIRED )
find_package( spdlog REQUIRED )
find_package( PkgConfig REQUIRED )
pkg_search_module(
  Libxmlpp
  REQUIRED
  IMPORTED_TARGET
  "libxml++-5.0" )

# prepare Version.cpp
configure_file( Version.hpp.in Version.hpp )

add_library( arinc_665 )

include( GenerateExportHeader )
generate_export_header( arinc_665 )

target_sources(
  arinc_665

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Arinc665.hpp
        Arinc665Exception.hpp
        FileTypeDescription.hpp
        MediumNumber.hpp
        PartNumber.hpp
        SupportedArinc665VersionDescription.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/arinc_665_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp

  PRIVATE
    Arinc665.cpp
    FileTypeDescription.cpp
    MediumNumber.cpp
    PartNumber.cpp
    SupportedArinc665VersionDescription.cpp )

target_compile_features( arinc_665 PUBLIC cxx_std_23 )

target_compile_definitions(
  arinc_665

  PRIVATE
    # Activate STL assertions
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:_GLIBCXX_ASSERTIONS>
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:_GLIBCXX_ASSERTIONS> )

target_compile_options(
  arinc_665

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
  arinc_665

  PUBLIC
    Boost::boost
    helper
    arinc_649

  PRIVATE
    spdlog::spdlog_header_only
    PkgConfig::Libxmlpp )

add_library( arinc_665_test OBJECT )

target_sources(
  arinc_665_test

  PRIVATE
    test/MediumNumber_decrementOperatorTest.cpp
    test/MediumNumber_incrementOperatorTest.cpp
    test/MediumNumberTest.cpp
    test/PartNumberTest.cpp
    test/VersionTest.cpp )

target_compile_features( arinc_665_test PUBLIC cxx_std_23 )

target_compile_definitions(
  arinc_665_test

  PRIVATE
    # Activate STL assertions
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:_GLIBCXX_ASSERTIONS>
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:_LIBCPP_DEBUG> )

target_compile_options(
  arinc_665_test

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
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic>
    # Disable new Clang-22 Warning for usage of c2y-extensions (__COUNTER__) within boost::test
    $<$<CXX_COMPILER_ID:Clang>:-Wno-c2y-extensions> )

target_link_libraries( arinc_665_test PUBLIC arinc_665 )

add_subdirectory( files )
add_subdirectory( media )
add_subdirectory( utils )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/arinc_665_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp )

install(
  TARGETS arinc_665
  RUNTIME_DEPENDENCY_SET arinc_665-runtime-deps
  EXPORT arinc_665-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT arinc_665-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
