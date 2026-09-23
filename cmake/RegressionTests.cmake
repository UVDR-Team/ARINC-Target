# SPDX-License-Identifier: MPL-2.0
file(GLOB_RECURSE regression_sources CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/lib/arinc_615a/*Test.cpp"
  "${CMAKE_SOURCE_DIR}/lib/tftp/*Test.cpp"
  "${CMAKE_SOURCE_DIR}/lib/arinc_support/*Test.cpp"
  "${CMAKE_SOURCE_DIR}/lib/arinc_checksum/*Test.cpp")
file(GLOB file_tests CONFIGURE_DEPENDS
  "${ARINC665_SOURCE_DIR}/lib/arinc_665/test/*Test.cpp"
  "${ARINC665_SOURCE_DIR}/lib/arinc_665/files/test/*Test.cpp")
add_executable(arinc_regression_tests tests/TestMain.cpp ${regression_sources} ${file_tests}
  lib/arinc_checksum/test/Arinc645TestData.cpp tests/RuntimeTest.cpp tests/HashVectorsTest.cpp tests/CInterfaceSmoke.c)
# The imported upstream test sources use C++20-only test conveniences such as
# std::to_array and std::ranges. Production libraries above remain C++17 and
# are already compiled before this host-only harness.
target_compile_features(arinc_regression_tests PRIVATE cxx_std_20)
target_precompile_headers(arinc_regression_tests PRIVATE
  "${CMAKE_SOURCE_DIR}/tests/compat/StandardLibrary.hpp")
target_include_directories(arinc_regression_tests PRIVATE "${CMAKE_SOURCE_DIR}/tests/compat")
target_link_libraries(arinc_regression_tests PRIVATE arinc_615a_tha_target)
add_test(NAME arinc_regression COMMAND arinc_regression_tests --report_level=short)
set_tests_properties(arinc_regression PROPERTIES TIMEOUT 90)
add_executable(arinc_host_runner tests/HostRunner.cpp)
target_link_libraries(arinc_host_runner PRIVATE arinc_615a_tha_target)
add_test(NAME arinc_self_test COMMAND arinc_host_runner --self-test)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_test(NAME arinc_network_transfers COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_SOURCE_DIR}/tests/network_smoke.py" --runner "$<TARGET_FILE:arinc_host_runner>")
set_tests_properties(arinc_network_transfers PROPERTIES TIMEOUT 120)
