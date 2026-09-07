# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

project(
  arinc_615a
  #VERSION 0.0.0.0
  DESCRIPTION "ARINC 615A Tool Suite"
  HOMEPAGE_URL "https://git.thomas-vogt.de/thomas-vogt/arinc_615a" )

# A DKM build must be self-contained.  In particular, do not invoke Git or
# fetch sources while configuring the target build.  The four protocol
# libraries below are supplied by the VxWorks integration as imported targets.
option(
  ARINC615A_DKM_PROFILE
  "Build only the target-side static library for a VxWorks DKM"
  ON )

set(
  ARINC615A_DEPENDENCY_FILE
  ""
  CACHE FILEPATH
  "CMake file that declares imported targets: tftp, helper, arinc_645 and arinc_665" )

if( ARINC615A_DEPENDENCY_FILE )
  if( NOT EXISTS "${ARINC615A_DEPENDENCY_FILE}" )
    message( FATAL_ERROR "ARINC615A_DEPENDENCY_FILE does not exist: ${ARINC615A_DEPENDENCY_FILE}" )
  endif()
  include( "${ARINC615A_DEPENDENCY_FILE}" )
endif()

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

enable_testing()

include( GNUInstallDirs )

# Activate Dynamic Libraries for Boost on Windows Platform
if( WIN32 )
  set( Boost_USE_STATIC_LIBS OFF )
endif()

# TODO Disable Module scanning for the moment
set( CMAKE_CXX_SCAN_FOR_MODULES OFF )

# find clang-tidy and add options to execute
find_program( CLANG_TIDY_EXE NAMES clang-tidy )

if( CLANG_TIDY_EXE )
  set( CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} -p )
endif()

# find iwyu and add options to execute
find_program( CLANG_IWYU_EXE NAMES iwyu )

if( CLANG_IWYU_EXE )
  #set( CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${CLANG_IWYU_EXE} )
endif()

foreach( dependency IN ITEMS tftp helper arinc_645 arinc_665 )
  if( NOT TARGET ${dependency} )
    message(
      FATAL_ERROR
      "Required target '${dependency}' was not supplied. Configure with "
      "-DARINC615A_DEPENDENCY_FILE=<your VxWorks imported-target file>. "
      "See VxWorksDKMDependencies.cmake.example." )
  endif()
endforeach()

if( ARINC615A_DKM_PROFILE )
  # Do not build host tools, Qt applications, command-line utilities or docs
  # into a kernel module.  They pull in desktop-only runtime dependencies.
  add_subdirectory( lib/arinc_615a )
  add_subdirectory( app/arinc_615a_unit_test/arinc_615a_test_tha )
else()
  message(
    FATAL_ERROR
    "The network-fetch desktop profile has been removed. Use the DKM profile "
    "with prebuilt/imported dependencies, or maintain a separate host build." )
endif()

install(
  FILES LICENSE
  RENAME ${PROJECT_NAME}.LICENSE
  TYPE DOC
  COMPONENT runtime )
