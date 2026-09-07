# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY MAN_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_operation-find.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_operation-list-targets.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_operation-information.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_operation-media-defined-download.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_operation-operator-defined-download.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_operation-upload.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_operation-adhoc-upload.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_operation-batch-upload.adoc )

find_program( ASCIIDOCTOR_EXE NAMES asciidoctor DOC "Ascii Doctor executable" )

if( NOT ASCIIDOCTOR_EXE )
  return()
endif()

get_property(
  MAN_PATHS
  DIRECTORY ${PROJECT_SOURCE_DIR}
  PROPERTY MAN_PATHS )

add_custom_target(
  arinc_615a_manpage
  COMMAND
    ${ASCIIDOCTOR_EXE}
    --backend manpage
    --source-dir ${CMAKE_CURRENT_SOURCE_DIR}
    --destination-dir ${CMAKE_CURRENT_BINARY_DIR}/man1
    ${MAN_PATHS}

  COMMENT "Generating ARINC 615A Manpages" )

# mandb --user-db ${CMAKE_CURRENT_BINARY_DIR}
# man -M ${CMAKE_CURRENT_BINARY_DIR}
# apropos -M ${CMAKE_CURRENT_BINARY_DIR}

add_dependencies( doc arinc_615a_manpage )

install(
  DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/man1
  TYPE MAN
  COMPONENT runtime
  OPTIONAL )
