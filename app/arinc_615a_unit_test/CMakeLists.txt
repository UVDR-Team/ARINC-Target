# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

add_executable( arinc_615a_unit_test )

target_sources( arinc_615a_unit_test PRIVATE arinc_615a_unit_test.cpp )

target_compile_features( arinc_615a_unit_test PRIVATE cxx_std_23 )

target_compile_options(
  arinc_615a_unit_test

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
  arinc_615a_unit_test

  PRIVATE
    arinc_615a_test
    arinc_665_test
    arinc_645_test
    tftp_test
    commands_test
    $<TARGET_NAME_IF_EXISTS:qt_icon_resources_test> # Optionally link qt_icon_resources_test
    helper_test )

#if( WIN32 AND BUILD_SHARED_LIBS )
#  add_custom_command(
#    TARGET arinc_615a_unit_test POST_BUILD
#    COMMAND
#      ${CMAKE_COMMAND} -E copy
#        $<TARGET_RUNTIME_DLLS:arinc_615a_unit_test>
#        $<TARGET_FILE_DIR:arinc_615a_unit_test>
#    COMMAND_EXPAND_LISTS )
#endif()

add_test(
  NAME arinc_615a_unit_test
  COMMAND arinc_615a_unit_test --log_format=JUNIT --log_sink=${CMAKE_BINARY_DIR}/arinc_615a_unit_test.junit.xml )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

install(
  TARGETS arinc_615a_unit_test
  RUNTIME_DEPENDENCY_SET arinc_615a-runtime-deps
  COMPONENT test )
