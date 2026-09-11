# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at
# http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

project(
  helper
  #VERSION 0.0.0.0
  DESCRIPTION "Helper Libraries"
  HOMEPAGE_URL "https://git.thomas-vogt.de/thomas-vogt/helper" )

find_package( Git QUIET )

if( GIT_FOUND )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --verify --short HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE PROJECT_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET )
endif()

if( PROJECT_GIT_HASH )
  set( PROJECT_VERSION_FULL "${PROJECT_VERSION} (${PROJECT_GIT_HASH})" )
else()
  set( PROJECT_VERSION_FULL "${PROJECT_VERSION} (vxworks)" )
endif()
string( STRIP "${PROJECT_VERSION_FULL}" PROJECT_VERSION_FULL )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

include( GNUInstallDirs )

# Activate Dynamic Libraries for Boost on Windows Platform
if( WIN32 )
  set( Boost_USE_STATIC_LIBS OFF )
endif()

# Disable Module scanning for the moment
set( CMAKE_CXX_SCAN_FOR_MODULES OFF )
set( CMAKE_CXX_EXTENSIONS OFF )

add_subdirectory( lib )

if( NOT ARINC_615A_VXWORKS )
  enable_testing()
  add_subdirectory( app )
  add_subdirectory( doc )
endif()

install(
  FILES LICENSE
  RENAME ${PROJECT_NAME}.LICENSE
  TYPE DOC
  COMPONENT runtime )

if( PROJECT_IS_TOP_LEVEL AND NOT ARINC_615A_VXWORKS )
  include( InstallPackage.cmake )
endif()
