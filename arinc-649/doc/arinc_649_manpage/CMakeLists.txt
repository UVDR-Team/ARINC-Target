# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_program( ASCIIDOCTOR_EXE NAMES asciidoctor DOC "Ascii Doctor executable" )

if( NOT ASCIIDOCTOR_EXE )
  return()
endif()

get_property(
  MAN_PATHS
  DIRECTORY ${PROJECT_SOURCE_DIR}
  PROPERTY MAN_PATHS )

add_custom_target(
  arinc_649_manpage
  COMMAND
    ${ASCIIDOCTOR_EXE}
    --backend manpage
    --source-dir ${CMAKE_CURRENT_SOURCE_DIR}
    --destination-dir ${CMAKE_CURRENT_BINARY_DIR}/man1
    ${MAN_PATHS}

  COMMENT "Generating ARINC 649 Manpages" )

# mandb --user-db ${CMAKE_CURRENT_BINARY_DIR}
# man -M ${CMAKE_CURRENT_BINARY_DIR}
# apropos -M ${CMAKE_CURRENT_BINARY_DIR}

add_dependencies( doc arinc_649_manpage )

install(
  DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/man1
  TYPE MAN
  COMPONENT runtime
  OPTIONAL )
