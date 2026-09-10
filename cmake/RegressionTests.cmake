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
target_link_libraries(arinc_regression_tests PRIVATE arinc_615a_tha_target)
add_test(NAME arinc_regression COMMAND arinc_regression_tests --report_level=short)
set_tests_properties(arinc_regression PROPERTIES TIMEOUT 90)
