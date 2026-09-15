# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at
# http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

project(
  arinc_615a
  #VERSION 0.0.0.0
  DESCRIPTION "ARINC 615A Tool Suite"
  HOMEPAGE_URL "https://git.thomas-vogt.de/thomas-vogt/arinc_615a" )

option( ARINC_615A_VXWORKS "Build ARINC 615A THA for VxWorks DKM" OFF )

# The default deliverable is the target library. Desktop tools are opt-in.
option( ARINC615A_TARGET_ONLY "Build the offline target stack only" ON )
if(CMAKE_SYSTEM_NAME STREQUAL "VxWorks")
  set(ARINC_615A_VXWORKS ON)
endif()
if(ARINC615A_TARGET_ONLY OR ARINC_615A_VXWORKS)
  include(cmake/TargetBuild.cmake)
  return()
endif()

# Optional Git integration: fallback if Git is not present
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

# Disable Module scanning
set( CMAKE_CXX_SCAN_FOR_MODULES OFF )
set( CMAKE_CXX_EXTENSIONS OFF )

# Include compatibility directory for platforms lacking <format> or spdlog
# Desktop builds use their native format and spdlog installations.

if( ARINC_615A_VXWORKS )
  add_compile_definitions( ARINC_615A_NO_PROGRAM_OPTIONS )
  # Add compatibility defines for VxWorks kernel / DKM build
  add_compile_definitions( BOOST_ASIO_NO_DEPRECATED )
  # Set C++ standard
  set( CMAKE_CXX_STANDARD 17 )
  set( CMAKE_CXX_STANDARD_REQUIRED ON )
endif()

include( FetchContent )

# helper
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/helper )
  set( FETCHCONTENT_SOURCE_DIR_HELPER ${CMAKE_SOURCE_DIR}/helper )
elseif( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../helper )
  set( FETCHCONTENT_SOURCE_DIR_HELPER ${CMAKE_SOURCE_DIR}/../helper )
endif()
FetchContent_Declare(
  helper
  GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/helper.git
  GIT_TAG        main )

# qt_icon_resources
if( NOT ARINC_615A_VXWORKS )
  if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/qt_icon_resources )
    set( FETCHCONTENT_SOURCE_DIR_QT_ICON_RESOURCES ${CMAKE_SOURCE_DIR}/qt_icon_resources )
  elseif( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../qt_icon_resources )
    set( FETCHCONTENT_SOURCE_DIR_QT_ICON_RESOURCES ${CMAKE_SOURCE_DIR}/../qt_icon_resources )
  endif()
  FetchContent_Declare(
    qt_icon_resources
    GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/qt_icon_resources.git
    GIT_TAG        main )
endif()

# arinc_649
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/arinc-649 )
  set( FETCHCONTENT_SOURCE_DIR_ARINC_649 ${CMAKE_SOURCE_DIR}/arinc-649 )
elseif( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../arinc-649 )
  set( FETCHCONTENT_SOURCE_DIR_ARINC_649 ${CMAKE_SOURCE_DIR}/../arinc-649 )
endif()
FetchContent_Declare(
  arinc_649
  GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/arinc-649.git
  GIT_TAG        main )

# arinc_665
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/arinc_665 )
  set( FETCHCONTENT_SOURCE_DIR_ARINC_665 ${CMAKE_SOURCE_DIR}/arinc_665 )
elseif( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../arinc_665 )
  set( FETCHCONTENT_SOURCE_DIR_ARINC_665 ${CMAKE_SOURCE_DIR}/../arinc_665 )
endif()
FetchContent_Declare(
  arinc_665
  GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/arinc_665.git
  GIT_TAG        main )

# tftp
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/tftp )
  set( FETCHCONTENT_SOURCE_DIR_TFTP ${CMAKE_SOURCE_DIR}/tftp )
elseif( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../tftp )
  set( FETCHCONTENT_SOURCE_DIR_TFTP ${CMAKE_SOURCE_DIR}/../tftp )
endif()
FetchContent_Declare(
  tftp
  GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/tftp.git
  GIT_TAG        main )

# commands
if( NOT ARINC_615A_VXWORKS )
  if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/commands )
    set( FETCHCONTENT_SOURCE_DIR_COMMANDS ${CMAKE_SOURCE_DIR}/commands )
  elseif( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../commands )
    set( FETCHCONTENT_SOURCE_DIR_COMMANDS ${CMAKE_SOURCE_DIR}/../commands )
  endif()
  FetchContent_Declare(
    commands
    GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/commands.git
    GIT_TAG        main )
endif()

if( ARINC_615A_VXWORKS )
  FetchContent_MakeAvailable( helper arinc_649 arinc_665 tftp )
else()
  FetchContent_MakeAvailable( helper qt_icon_resources arinc_649 arinc_665 tftp commands )
endif()

if( NOT ARINC_615A_VXWORKS )
  enable_testing()

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
endif()

add_subdirectory( lib )
add_subdirectory( app )

if( NOT ARINC_615A_VXWORKS )
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
