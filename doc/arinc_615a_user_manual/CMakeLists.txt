# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_program( ASCIIDOCTOR_EXE NAMES asciidoctor DOC "Ascii Doctor executable" )

if( NOT ASCIIDOCTOR_EXE )
  return()
endif()

add_custom_target(
  arinc_615a_user_manual
  COMMAND
    ${ASCIIDOCTOR_EXE}
    --backend html5
    --source-dir ${CMAKE_CURRENT_SOURCE_DIR}
    --destination-dir ${CMAKE_CURRENT_BINARY_DIR}
    --attribute source-highlighter=pygments
    --attribute arinc_665_doc_src_dir=${arinc_665_SOURCE_DIR}/doc/arinc_665_user_manual
    --attribute tftp_doc_src_dir=${tftp_SOURCE_DIR}/doc/tftp_user_manual
    ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_user_manual.adoc
  COMMENT "Generating arinc_615a_user_manual.html" )

add_dependencies( doc arinc_615a_user_manual )

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/arinc_615a_user_manual.html
  TYPE DOC
  COMPONENT runtime
  OPTIONAL )
