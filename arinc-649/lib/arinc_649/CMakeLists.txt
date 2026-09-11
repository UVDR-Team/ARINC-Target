# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

if( ARINC_615A_VXWORKS )
  find_package( Boost REQUIRED )
else()
  find_package( Boost 1.88 REQUIRED )
  find_package( spdlog REQUIRED )
endif()

# prepare Version.cpp
configure_file( Version.hpp.in Version.hpp )

add_library( arinc_649 )

include( GenerateExportHeader )
generate_export_header( arinc_649 )

target_sources(
  arinc_649

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        Arinc649.hpp
        Arinc649Crc.hpp
        Arinc649Exception.hpp
        CheckValue.hpp
        CheckValueGenerator.hpp
        CheckValueTypeDescription.hpp
        Filename.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/arinc_649_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp

  PRIVATE
    Arinc649.cpp
    CheckValue.cpp
    CheckValueGenerator.cpp
    CheckValueTypeDescription.cpp
    Filename.cpp

    implementation/CrcCheckValueGenerator.hpp
    implementation/CrcCheckValueGenerator.ipp
    implementation/HashCheckValueGenerator.hpp
    implementation/HashCheckValueGenerator.ipp
    implementation/NopCheckValueGenerator.hpp
    implementation/NopCheckValueGenerator.cpp )

if( ARINC_615A_VXWORKS )
  target_compile_features( arinc_649 PUBLIC cxx_std_17 )
else()
  target_compile_features( arinc_649 PUBLIC cxx_std_23 )
endif()

target_compile_definitions(
  arinc_649

  PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:_GLIBCXX_ASSERTIONS>
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:_GLIBCXX_ASSERTIONS> )

target_compile_options(
  arinc_649

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4275>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  arinc_649

  PUBLIC
    helper
    Boost::boost
    $<$<NOT:$<BOOL:${ARINC_615A_VXWORKS}>>:spdlog::spdlog_header_only> )

if( NOT ARINC_615A_VXWORKS )
  add_library( arinc_649_test OBJECT )

  target_sources(
    arinc_649_test

    PRIVATE
      test/Arinc649Crc8Test.cpp
      test/Arinc649Crc16Test.cpp
      test/Arinc649Crc32Test.cpp
      test/Arinc649Crc64Test.cpp
      test/Arinc649CrcTest.hpp
      test/Arinc649TestData.hpp
      test/Arinc649TestData.cpp
      test/CheckValueGeneratorCrc8Test.cpp
      test/CheckValueGeneratorCrc16Test.cpp
      test/CheckValueGeneratorCrc32Test.cpp
      test/CheckValueGeneratorCrc64Test.cpp
      test/CheckValueGeneratorTest.cpp
      test/CheckValueTest.cpp
      test/FilenameTest.cpp
      test/VersionTest.cpp )

  target_compile_features( arinc_649_test PUBLIC cxx_std_23 )

  target_compile_definitions(
    arinc_649_test

    PRIVATE
      $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:_GLIBCXX_ASSERTIONS>
      $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Debug>>:_LIBCPP_DEBUG> )

  target_compile_options(
    arinc_649_test

    PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:/W4>
      $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
      $<$<CXX_COMPILER_ID:MSVC>:/wd4275>

      $<$<CXX_COMPILER_ID:GNU>:-Wall>
      $<$<CXX_COMPILER_ID:GNU>:-Wextra>
      $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

      $<$<CXX_COMPILER_ID:Clang>:-Wall>
      $<$<CXX_COMPILER_ID:Clang>:-Wextra>
      $<$<CXX_COMPILER_ID:Clang>:-Wpedantic>
      $<$<CXX_COMPILER_ID:Clang>:-Wno-c2y-extensions> )

  target_link_libraries( arinc_649_test PUBLIC arinc_649 )
endif()

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/arinc_649_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp )

install(
  TARGETS arinc_649
  RUNTIME_DEPENDENCY_SET arinc_649-runtime-deps
  EXPORT arinc_649-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT arinc_649-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
