# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

find_package( Doxygen )
find_package( Boost )

if( NOT Doxygen_FOUND )
  return()
endif()

get_property(
  HELPER_DOC_PATHS
  DIRECTORY ${helper_SOURCE_DIR}
  PROPERTY DOC_PATHS )

get_property(
  QT_ICON_RESOURCES_DOC_PATHS
  DIRECTORY ${qt_icon_resources_SOURCE_DIR}
  PROPERTY DOC_PATHS )

get_property(
  COMMANDS_DOC_PATHS
  DIRECTORY ${commands_SOURCE_DIR}
  PROPERTY DOC_PATHS )

get_property(
  ARINC_649_DOC_PATHS
  DIRECTORY ${arinc_649_SOURCE_DIR}
  PROPERTY DOC_PATHS )

get_property(
  ARINC_665_DOC_PATHS
  DIRECTORY ${arinc_665_SOURCE_DIR}
  PROPERTY DOC_PATHS )

set( DOXYGEN_PROJECT_NAME "${arinc_665_DESCRIPTION}" )
set( DOXYGEN_PROJECT_BRIEF "Internal API Description" )
set( DOXYGEN_PROJECT_NUMBER "${arinc_665_VERSION}" )
set( DOXYGEN_BUILTIN_STL_SUPPORT YES )
set( DOXYGEN_EXTRACT_PRIVATE YES )
set( DOXYGEN_EXTRACT_STATIC YES )
set( DOXYGEN_EXTRACT_PACKAGE YES )
set( DOXYGEN_QUIET YES )
#set( DOXYGEN_WARN_NO_PARAMDOC YES )
set( DOXYGEN_HTML_DYNAMIC_SECTIONS YES )
set( DOXYGEN_GENERATE_TREEVIEW YES )
set( DOXYGEN_INCLUDED_BY_GRAPH NO )

if( WIN32 )
  # shorten filename otherwise exceeds 255 characters for paths
  set( DOXYGEN_SHORT_NAMES YES )
endif()
# restrict number of parallel dot-tasks (parallelisation performed by cmake)
set( DOXYGEN_DOT_NUM_THREADS 1 )

set(
  DOXYGEN_EXCLUDE_PATTERNS

  */boost/* )

set(
  DOXYGEN_STRIP_FROM_PATH

  ${helper_SOURCE_DIR}/lib
  ${helper_BINARY_DIR}/lib
  ${helper_SOURCE_DIR}/app
  ${qt_icon_resources_SOURCE_DIR}/lib
  ${qt_icon_resources_BINARY_DIR}/lib
  ${qt_icon_resources_SOURCE_DIR}/app
  ${commands_SOURCE_DIR}/lib
  ${commands_BINARY_DIR}/lib
  ${commands_SOURCE_DIR}/app
  ${arinc_649_SOURCE_DIR}/lib
  ${arinc_649_BINARY_DIR}/lib
  ${arinc_649_SOURCE_DIR}/app
  ${arinc_665_SOURCE_DIR}/lib
  ${arinc_665_BINARY_DIR}/lib
  ${arinc_665_SOURCE_DIR}/app )

set(
  DOXYGEN_INCLUDE_PATH

  ${Boost_INCLUDE_DIRS}
  ${helper_SOURCE_DIR}/lib
  ${helper_BINARY_DIR}/lib
  ${qt_icon_resources_SOURCE_DIR}/lib
  ${qt_icon_resources_BINARY_DIR}/lib
  ${commands_SOURCE_DIR}/lib
  ${commands_BINARY_DIR}/lib
  ${arinc_649_SOURCE_DIR}/lib
  ${arinc_649_BINARY_DIR}/lib
  ${arinc_665_SOURCE_DIR}/lib
  ${arinc_665_BINARY_DIR}/lib )

doxygen_add_docs(
  arinc_665_internal_doc

  ../Mainpage.md
  ${HELPER_DOC_PATHS}
  ${QT_ICON_RESOURCES_DOC_PATHS}
  ${COMMANDS_DOC_PATHS}
  ${ARINC_649_DOC_PATHS}
  ${ARINC_665_DOC_PATHS} )

add_dependencies( doc arinc_665_internal_doc )

# add index.html redirection
set( INDEX_HTML_OUTPUT_DIR html )
configure_file(
  ../doxygen_index.html.in
  index.html
  @ONLY )

install(
  DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/html/
  DESTINATION ${CMAKE_INSTALL_DOCDIR}/arinc_665_internal_doc
  COMPONENT development
  OPTIONAL
  MESSAGE_NEVER )

set( INDEX_HTML_OUTPUT_DIR arinc_665_internal_doc )
configure_file(
  ../doxygen_index.html.in
  arinc_665_internal_doc.html
  @ONLY )

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/arinc_665_internal_doc.html
  TYPE DOC
  COMPONENT development
  OPTIONAL )
