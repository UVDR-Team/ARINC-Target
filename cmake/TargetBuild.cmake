# SPDX-License-Identifier: MPL-2.0
include(GNUInstallDirs)
include(GenerateExportHeader)
include(${CMAKE_CURRENT_LIST_DIR}/TargetSources.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(ARINC615A_BOOST_INCLUDE_DIR "" CACHE PATH "Directory containing SDK boost/version.hpp (Boost >= 1.86)")
if(NOT ARINC615A_BOOST_INCLUDE_DIR)
  find_path(ARINC615A_BOOST_HEADERS boost/version.hpp)
  if(NOT ARINC615A_BOOST_HEADERS)
    message(FATAL_ERROR "Set ARINC615A_BOOST_INCLUDE_DIR to the directory containing the SDK boost/ headers. No Boost binaries are needed.")
  endif()
  set(ARINC615A_BOOST_INCLUDE_DIR "${ARINC615A_BOOST_HEADERS}")
endif()
if(NOT EXISTS "${ARINC615A_BOOST_INCLUDE_DIR}/boost/version.hpp")
  message(FATAL_ERROR "ARINC615A_BOOST_INCLUDE_DIR must contain boost/version.hpp")
endif()
file(STRINGS "${ARINC615A_BOOST_INCLUDE_DIR}/boost/version.hpp" boost_version_line REGEX "^#define BOOST_VERSION [0-9]+")
string(REGEX REPLACE ".*BOOST_VERSION ([0-9]+).*" "\\1" boost_version "${boost_version_line}")
if(boost_version LESS 108600)
  message(FATAL_ERROR "This target profile requires Boost headers >= 1.86; found ${boost_version}")
endif()

if(ARINC_615A_VXWORKS)
  set(background_default OFF)
else()
  set(background_default ON)
endif()
option(ARINC615A_BACKGROUND_THREAD "Allow the C API to create a std::thread (DKM normally uses its own task)" ${background_default})
option(ARINC615A_BUILD_TESTS "Build native protocol and lifecycle regression tests" OFF)

add_library(arinc_target_platform INTERFACE)
target_compile_features(arinc_target_platform INTERFACE cxx_std_20)
target_include_directories(arinc_target_platform INTERFACE
  ${PROJECT_SOURCE_DIR}/compat)
target_include_directories(arinc_target_platform SYSTEM INTERFACE ${ARINC615A_BOOST_INCLUDE_DIR})
target_compile_definitions(arinc_target_platform INTERFACE
  ARINC_615A_NO_PROGRAM_OPTIONS BOOST_ASIO_NO_DEPRECATED
  BOOST_ERROR_CODE_HEADER_ONLY BOOST_SYSTEM_NO_LIB BOOST_ALL_NO_LIB
  FMT_HEADER_ONLY=1 ARINC615A_TARGET_BUILD=1)
if(ARINC615A_BACKGROUND_THREAD)
  target_compile_definitions(arinc_target_platform INTERFACE ARINC615A_BACKGROUND_THREAD=1)
endif()
# VxWorks toolchain owns kernel flags and runtime linkage. Never inject host pthread libraries.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "VxWorks")
  find_package(Threads REQUIRED)
  target_link_libraries(arinc_target_platform INTERFACE Threads::Threads)
else()
  target_compile_options(arinc_target_platform INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:-include>"
    "$<$<COMPILE_LANGUAGE:CXX>:${PROJECT_SOURCE_DIR}/compat/vxworks_platform.hpp>")
endif()

set(PROJECT_VERSION_FULL "vxworks-dependency-audit")
function(arinc_target_library name source_dir)
  add_library(${name} STATIC ${${name}_SOURCES})
  set_target_properties(${name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
  target_link_libraries(${name} PUBLIC arinc_target_platform)
  target_include_directories(${name} PUBLIC "${PROJECT_SOURCE_DIR}/${source_dir}/.." "${PROJECT_BINARY_DIR}/generated")
  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/generated/${name}")
  generate_export_header(${name} EXPORT_FILE_NAME "${PROJECT_BINARY_DIR}/generated/${name}/${name}_export.h")
  set(PROJECT_NAME "${name}")
  set(PROJECT_DESCRIPTION "${name} target library")
  set(PROJECT_GIT_HASH "source")
  configure_file("${PROJECT_SOURCE_DIR}/${source_dir}/Version.hpp.in" "${PROJECT_BINARY_DIR}/generated/${name}/Version.hpp")
endfunction()

arinc_target_library(helper helper/lib/helper)
arinc_target_library(arinc_649 arinc-649/lib/arinc_649)
arinc_target_library(arinc_665 arinc_665/lib/arinc_665)
arinc_target_library(tftp tftp/lib/tftp)
arinc_target_library(arinc_615a lib/arinc_615a)
target_link_libraries(arinc_649 PUBLIC helper)
target_link_libraries(arinc_665 PUBLIC arinc_649 helper)
target_link_libraries(tftp PUBLIC helper)
target_link_libraries(arinc_615a PUBLIC tftp arinc_649 helper)
add_library(arinc_615a_test_tha_lib STATIC ${arinc_615a_test_tha_lib_SOURCES})
set_target_properties(arinc_615a_test_tha_lib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
target_include_directories(arinc_615a_test_tha_lib PUBLIC "${PROJECT_SOURCE_DIR}/app/arinc_615a_test_tha")
target_link_libraries(arinc_615a_test_tha_lib PUBLIC arinc_615a arinc_665)
add_library(arinc_615a_tha_target ALIAS arinc_615a_test_tha_lib)

install(TARGETS helper arinc_649 arinc_665 tftp arinc_615a arinc_615a_test_tha_lib ARCHIVE DESTINATION lib)
install(FILES "${PROJECT_SOURCE_DIR}/app/arinc_615a_test_tha/arinc_615a_tha.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/app/arinc_615a_test_tha/arinc_615a_test_tha.json.template" DESTINATION share/arinc615a)
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION share/arinc615a)
file(WRITE "${PROJECT_BINARY_DIR}/BUILD_INFO.txt"
  "System: ${CMAKE_SYSTEM_NAME}\nProcessor: ${CMAKE_SYSTEM_PROCESSOR}\nCross compiling: ${CMAKE_CROSSCOMPILING}\nCompiler: ${CMAKE_CXX_COMPILER}\nCompiler version: ${CMAKE_CXX_COMPILER_VERSION}\nToolchain: ${CMAKE_TOOLCHAIN_FILE}\nBoost: ${boost_version}\nC++ standard: 20\nBackground thread: ${ARINC615A_BACKGROUND_THREAD}\n")
install(FILES "${PROJECT_BINARY_DIR}/BUILD_INFO.txt" DESTINATION share/arinc615a)
install(FILES "${PROJECT_BINARY_DIR}/DEPENDENCY_AUDIT.txt" DESTINATION share/arinc615a OPTIONAL)
add_custom_target(arinc615a_audit
  COMMAND ${CMAKE_COMMAND}
    "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}" "-DBUILD_DIR=${PROJECT_BINARY_DIR}"
    "-DNM=${CMAKE_NM}" "-DSUFFIX=${CMAKE_STATIC_LIBRARY_SUFFIX}"
    "-DGENERATOR=${CMAKE_GENERATOR}" "-DMAKE_TOOL=${CMAKE_MAKE_PROGRAM}"
    -P "${CMAKE_CURRENT_LIST_DIR}/AuditTarget.cmake"
  DEPENDS arinc_615a_test_tha_lib VERBATIM)

if(ARINC615A_BUILD_TESTS)
  if(CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Regression tests are native-only; disable ARINC615A_BUILD_TESTS for the DKM build")
  endif()
  enable_testing()
  include(${CMAKE_CURRENT_LIST_DIR}/TargetTests.cmake)
endif()
message(STATUS "Offline target only: helper, CRC/hash, ARINC 665 binary files, TFTP, ARINC 615A target, C API")
message(STATUS "Boost headers: ${boost_version}; no compiled Boost, Qt, XML, commands, GUI, host operations or network fetches")
