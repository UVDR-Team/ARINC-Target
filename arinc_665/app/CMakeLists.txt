# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/arinc_665_applications.md )

add_subdirectory( arinc_665_media_set_decompiler )
add_subdirectory( arinc_665_media_set_check )
add_subdirectory( arinc_665_media_set_compiler )
add_subdirectory( arinc_665_media_set_compiler_gui )
add_subdirectory( arinc_665_media_set_manager )
add_subdirectory( arinc_665_ls )

add_subdirectory( arinc_665_unit_test )

add_subdirectory( arinc_665_print_media_set )
add_subdirectory( arinc_665_print_xml )
add_subdirectory( arinc_665_print_load )

add_subdirectory( arinc_665_media_set_manager_gui )
add_subdirectory( arinc_665_media_set_viewer_gui )
