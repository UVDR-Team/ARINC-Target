# SPDX-License-Identifier: MPL-2.0
# Existing binary-format, packet and checksum tests; desktop XML/GUI tests excluded.
file(GLOB_RECURSE protocol_tests CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/lib/arinc_615a/*/test/*.cpp"
  "${PROJECT_SOURCE_DIR}/lib/arinc_615a/test/*.cpp"
  "${PROJECT_SOURCE_DIR}/tftp/lib/tftp/*/test/*.cpp"
  "${PROJECT_SOURCE_DIR}/tftp/lib/tftp/test/*.cpp"
  "${PROJECT_SOURCE_DIR}/arinc-649/lib/arinc_649/test/*.cpp"
  "${PROJECT_SOURCE_DIR}/arinc_665/lib/arinc_665/files/test/*.cpp"
  "${PROJECT_SOURCE_DIR}/arinc_665/lib/arinc_665/test/*.cpp"
  "${PROJECT_SOURCE_DIR}/helper/lib/helper/test/*.cpp")
list(FILTER protocol_tests EXCLUDE REGEX "(KnownDirectories|Scheduler|/host/|/find/clients/)")
add_executable(arinc615a_protocol_tests tests/TestMain.cpp tests/Portability.cpp ${protocol_tests})
# Upstream test-data generators use C++23; the libraries remain C++20.
target_compile_features(arinc615a_protocol_tests PRIVATE cxx_std_23)
target_link_libraries(arinc615a_protocol_tests PRIVATE arinc_615a_test_tha_lib)
add_test(NAME protocol_regressions COMMAND arinc615a_protocol_tests --log_level=message)

add_executable(arinc615a_link_check tests/LinkCheck.c)
set_target_properties(arinc615a_link_check PROPERTIES LINKER_LANGUAGE CXX LINK_LIBRARY_OVERRIDE WHOLE_ARCHIVE)
target_link_libraries(arinc615a_link_check PRIVATE
  "$<LINK_LIBRARY:WHOLE_ARCHIVE,arinc_615a_test_tha_lib,arinc_615a,tftp,arinc_665,arinc_649,helper>")
add_test(NAME all_target_objects_link COMMAND arinc615a_link_check)

add_executable(arinc615a_lifecycle_tests tests/Lifecycle.cpp)
target_link_libraries(arinc615a_lifecycle_tests PRIVATE arinc_615a_test_tha_lib)
add_test(NAME lifecycle_and_udp COMMAND arinc615a_lifecycle_tests)
set_tests_properties(lifecycle_and_udp PROPERTIES TIMEOUT 45)
