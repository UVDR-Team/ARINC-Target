# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

if( ARINC_615A_VXWORKS )
  find_package( Boost REQUIRED )
  include_directories( BEFORE ${CMAKE_SOURCE_DIR}/compat )
else()
  find_package( Boost REQUIRED COMPONENTS program_options )
  find_package( spdlog REQUIRED )
endif()

# prepare Version.cpp
configure_file( Version.hpp.in Version.hpp )

add_library( tftp )

include( GenerateExportHeader )
generate_export_header( tftp )

target_sources(
  tftp

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        DataHandler.hpp
        ReceiveDataHandler.hpp
        RequestTypeDescription.hpp
        Tftp.hpp
        TftpConfiguration.hpp
        TftpException.hpp
        TftpOptionsConfiguration.hpp
        TransferStatusDescription.hpp
        TransmitDataHandler.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/tftp_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp

  PRIVATE
    RequestTypeDescription.cpp
    Tftp.cpp
    TftpConfiguration.cpp
    TftpOptionsConfiguration.cpp
    TransferStatusDescription.cpp )

if( ARINC_615A_VXWORKS )
  target_compile_features( tftp PUBLIC cxx_std_17 )
else()
  target_compile_features( tftp PUBLIC cxx_std_23 )
endif()

target_compile_definitions(
  tftp

  PUBLIC
    BOOST_ASIO_NO_DEPRECATED

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING> )

target_compile_options(
  tftp

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4275>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4702>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4996>
    $<$<CXX_COMPILER_ID:MSVC>:/external:templates->

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  tftp

  PUBLIC
    helper
    $<$<NOT:$<BOOL:${ARINC_615A_VXWORKS}>>:Boost::program_options>

  PRIVATE
    $<$<NOT:$<BOOL:${ARINC_615A_VXWORKS}>>:spdlog::spdlog_header_only>
    $<$<PLATFORM_ID:Windows>:wsock32>
    $<$<PLATFORM_ID:Windows>:ws2_32> )

if( NOT ARINC_615A_VXWORKS )
  add_library( tftp_test OBJECT )

  target_sources(
    tftp_test

    PRIVATE
      test/TftpOptionsConfigurationTest.cpp
      test/VersionTest.cpp )

  target_compile_features( tftp_test PUBLIC cxx_std_23 )
  target_link_libraries( tftp_test PUBLIC tftp )
endif()

add_subdirectory( files )
add_subdirectory( packets )
add_subdirectory( clients )
add_subdirectory( servers )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/tftp_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp )

install(
  TARGETS tftp
  RUNTIME_DEPENDENCY_SET tftp-runtime-deps
  EXPORT tftp-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT tftp-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
