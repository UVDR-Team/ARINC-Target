# SPDX-License-Identifier: MPL-2.0
include_guard(GLOBAL)
include(GenerateExportHeader)

function(arinc_configure_support source_root)
  set(ARINC_SUPPORT_ROOT "${source_root}" CACHE INTERNAL "Local shared source root")
  set(ARINC_BOOST_INCLUDE_DIR "" CACHE PATH "Boost headers from the matching VxWorks SDK (>= 1.88)")
  if(NOT ARINC_BOOST_INCLUDE_DIR)
    find_path(ARINC_DISCOVERED_BOOST_INCLUDE_DIR boost/version.hpp REQUIRED)
    set(ARINC_BOOST_INCLUDE_DIR "${ARINC_DISCOVERED_BOOST_INCLUDE_DIR}" CACHE PATH "Boost headers" FORCE)
  endif()
  if(NOT EXISTS "${ARINC_BOOST_INCLUDE_DIR}/boost/version.hpp")
    message(FATAL_ERROR "ARINC_BOOST_INCLUDE_DIR must contain boost/version.hpp; no dependencies will be downloaded.")
  endif()
  file(STRINGS "${ARINC_BOOST_INCLUDE_DIR}/boost/version.hpp" version_line REGEX "^#define BOOST_VERSION [0-9]+$")
  string(REGEX REPLACE ".* ([0-9]+)$" "\\1" boost_version "${version_line}")
  if(boost_version LESS 108800)
    message(FATAL_ERROR "Boost >= 1.88 is needed for the original MD5/SHA implementations (Boost.Hash2). Supplied: ${boost_version}.")
  endif()
  add_library(arinc_build_config INTERFACE)
  target_compile_features(arinc_build_config INTERFACE cxx_std_23)
  target_compile_definitions(arinc_build_config INTERFACE BOOST_ASIO_NO_DEPRECATED
    ARINC_ENABLE_COMMAND_LINE=$<BOOL:${ARINC_ENABLE_COMMAND_LINE}>)
  target_include_directories(arinc_build_config INTERFACE
    "$<BUILD_INTERFACE:${source_root}/lib>"
    "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>" "$<INSTALL_INTERFACE:include>")
  # Never bake an SDK- or workstation-specific Boost path into the installed
  # CMake export. Consumers of the installed archives provide their matching
  # VxWorks SDK Boost include path explicitly.
  target_include_directories(arinc_build_config SYSTEM INTERFACE
    "$<BUILD_INTERFACE:${ARINC_BOOST_INCLUDE_DIR}>")
  if(WIN32)
    target_link_libraries(arinc_build_config INTERFACE ws2_32)
  elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "VxWorks")
    find_package(Threads REQUIRED)
    target_link_libraries(arinc_build_config INTERFACE Threads::Threads)
  endif()
  if(ARINC_ENABLE_COMMAND_LINE)
    if(TARGET Boost::program_options)
      target_link_libraries(arinc_build_config INTERFACE Boost::program_options)
    elseif(boost_version EQUAL 108800)
      file(GLOB cli_sources CONFIGURE_DEPENDS "${source_root}/third_party/boost_program_options/src/*.cpp")
      add_library(arinc_cli_options STATIC ${cli_sources})
      target_include_directories(arinc_cli_options SYSTEM PRIVATE "${ARINC_BOOST_INCLUDE_DIR}")
      target_compile_features(arinc_cli_options PRIVATE cxx_std_23)
      target_link_libraries(arinc_build_config INTERFACE arinc_cli_options)
    else()
      message(FATAL_ERROR "Desktop CLI: bundled Program_options sources match Boost 1.88. Supply matching headers or a local static Boost::program_options target. DKM builds do not need this component.")
    endif()
  endif()
endfunction()

function(arinc_generated_headers target source_dir)
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include/${target}")
  generate_export_header(${target} EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/include/${target}/${target}_export.h")
  # Fixed, local metadata. Configuration does not execute Git.
  set(PROJECT_NAME "${target}")
  set(PROJECT_VERSION "")
  set(PROJECT_VERSION_FULL "offline-source-snapshot")
  set(PROJECT_GIT_HASH "")
  configure_file("${source_dir}/Version.hpp.in" "${CMAKE_BINARY_DIR}/include/${target}/Version.hpp" @ONLY)
  target_link_libraries(${target} PUBLIC arinc_build_config)
endfunction()

function(arinc_add_665 source_dir)
  if(NOT EXISTS "${source_dir}/lib/arinc_665/files/LoadHeaderFile.cpp")
    message(FATAL_ERROR "ARINC665_SOURCE_DIR does not contain the local ARINC 665 library sources.")
  endif()
  # Preserve binary file support; XML/Qt/media-manager facilities are desktop code.
  file(GLOB core CONFIGURE_DEPENDS "${source_dir}/lib/arinc_665/*.cpp" "${source_dir}/lib/arinc_665/files/*.cpp")
  # These upstream files contain only an empty namespace and produce empty
  # archive members (and ranlib warnings on some toolchains).
  list(FILTER core EXCLUDE REGEX "/(BatchLoadInfo|BatchTargetInfo)\\.cpp$")
  add_library(arinc_665 STATIC ${core}
    "${ARINC_SUPPORT_ROOT}/lib/arinc_checksum/CheckValue.cpp"
    "${ARINC_SUPPORT_ROOT}/lib/arinc_checksum/CheckValueGenerator.cpp"
    "${ARINC_SUPPORT_ROOT}/lib/arinc_checksum/CheckValueTypeDescription.cpp"
    "${ARINC_SUPPORT_ROOT}/lib/arinc_checksum/implementation/NopCheckValueGenerator.cpp")
  target_include_directories(arinc_665 PUBLIC "$<BUILD_INTERFACE:${source_dir}/lib>" "$<INSTALL_INTERFACE:include>")
  arinc_generated_headers(arinc_665 "${source_dir}/lib/arinc_665")
endfunction()

function(arinc_add_tftp)
  set(base "${ARINC_SUPPORT_ROOT}/lib/tftp")
  file(GLOB sources CONFIGURE_DEPENDS
    "${base}/*.cpp" "${base}/files/*.cpp" "${base}/packets/*.cpp"
    "${base}/clients/*.cpp" "${base}/clients/implementation/*.cpp"
    "${base}/servers/*.cpp" "${base}/servers/implementation/*.cpp")
  add_library(tftp STATIC ${sources})
  arinc_generated_headers(tftp "${base}")
endfunction()

function(arinc_add_615a)
  set(base "${ARINC_SUPPORT_ROOT}/lib/arinc_615a")
  file(GLOB sources CONFIGURE_DEPENDS
    "${base}/*.cpp" "${base}/information/*.cpp" "${base}/files/*.cpp"
    "${base}/files/download/*.cpp" "${base}/files/information/*.cpp" "${base}/files/upload/*.cpp"
    "${base}/find/*.cpp" "${base}/find/packets/*.cpp"
    "${base}/find/servers/*.cpp" "${base}/find/servers/implementation/*.cpp"
    "${base}/tftp/*.cpp" "${base}/tftp/clients/*.cpp" "${base}/tftp/clients/implementation/*.cpp"
    "${base}/tftp/servers/*.cpp" "${base}/tftp/servers/implementation/*.cpp"
    "${base}/target/*.cpp" "${base}/target/implementation/*.cpp")
  add_library(arinc_615a STATIC ${sources})
  arinc_generated_headers(arinc_615a "${base}")
  target_link_libraries(arinc_615a PUBLIC tftp arinc_665)
  if(ARINC_ENABLE_COMMAND_LINE)
    target_sources(arinc_615a PRIVATE "${ARINC_SUPPORT_ROOT}/lib/arinc_support/BoostAsioProgramOptions.cpp")
  endif()
endfunction()
