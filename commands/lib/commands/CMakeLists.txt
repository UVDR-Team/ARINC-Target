# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_package( Boost REQUIRED COMPONENTS program_options )

# prepare Version.cpp
configure_file( Version.hpp.in Version.hpp )

add_library( commands )

include( GenerateExportHeader )
generate_export_header( commands )

target_sources(
  commands

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Commands.hpp
        CommandRegistry.hpp
        Utils.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/commands_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp

  PRIVATE
    Commands.cpp
    CommandRegistry.cpp
    Utils.cpp

    implementation/CommandLineHandlerImpl.hpp
    implementation/CommandLineHandlerImpl.cpp
    implementation/CommandRegistryImpl.hpp
    implementation/CommandRegistryImpl.cpp
    implementation/EchoCommandImpl.hpp
    implementation/EchoCommandImpl.cpp )

target_compile_features( commands PUBLIC cxx_std_23 )

target_compile_options(
  commands

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries( commands PUBLIC helper )

add_library( commands_test OBJECT )

target_sources(
  commands_test

  PRIVATE
    test/CommandUtilsTest.cpp
    test/VersionTest.cpp )

target_compile_features( commands_test PUBLIC cxx_std_23 )

target_compile_options(
  commands_test

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

target_link_libraries( commands_test PUBLIC commands )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/commands_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp )

install(
  TARGETS commands
  RUNTIME_DEPENDENCY_SET commands-runtime-deps
  EXPORT commands-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT commands-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
