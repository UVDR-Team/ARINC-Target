# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

add_executable( helper_unit_test )

target_sources( helper_unit_test PRIVATE helper_unit_test.cpp )

target_compile_features( helper_unit_test PRIVATE cxx_std_23 )

target_compile_options(
  helper_unit_test

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries( helper_unit_test PRIVATE helper_test )

add_test(
  NAME helper_unit_test
  COMMAND helper_unit_test --log_format=JUNIT --log_sink=${CMAKE_BINARY_DIR}/helper_unit_test.junit.xml )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

install(
  TARGETS helper_unit_test
  RUNTIME_DEPENDENCY_SET helper-runtime-deps
  COMPONENT test )
