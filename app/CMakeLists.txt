# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/arinc_615a_applications.md )

add_subdirectory( arinc_615a_operation )
add_subdirectory( arinc_615a_download_request_file )
add_subdirectory( arinc_615a_data_loader_gui )
add_subdirectory( arinc_615a_unit_test )
add_subdirectory( arinc_615a_unit_test/arinc_615a_test_tha )
