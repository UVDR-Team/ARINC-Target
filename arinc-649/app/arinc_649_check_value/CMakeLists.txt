# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_package( Boost REQUIRED COMPONENTS program_options )

add_executable( arinc_649_check_value )

target_sources(
  arinc_649_check_value
  PRIVATE
    arinc_649_check_value.cpp )

target_compile_features( arinc_649_check_value PRIVATE cxx_std_23 )

target_compile_options(
  arinc_649_check_value

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>
    # Disable Warning for exporting classes with std::* private members
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries( arinc_649_check_value PRIVATE arinc_649 Boost::program_options )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY MAN_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_649_check_value.adoc )

install(
  TARGETS arinc_649_check_value
  RUNTIME_DEPENDENCY_SET arinc_649-runtime-deps
  COMPONENT runtime )

# Copy Own DLLs on Windows and Shared build to App directory for direct
# execution.
# TODO $<TARGET_RUNTIME_DLLS:arinc_649_check_value> doesn't
#      work - Boost defines wrong paths ('lib/' <-> 'bin'/)
if( WIN32 AND BUILD_SHARED_LIBS )
  add_custom_command(
    TARGET arinc_649_check_value
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      -E copy
      -t
      $<TARGET_FILE_DIR:arinc_649_check_value>
      #$<TARGET_RUNTIME_DLLS:arinc_649_check_value>
      $<TARGET_FILE:helper>
      $<TARGET_FILE:arinc_649>
    COMMAND_EXPAND_LISTS )
endif()
