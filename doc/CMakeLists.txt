# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.3 )

if( NOT TARGET doc )
  add_custom_target( doc COMMENT "Generate Documentation" )
endif()

add_subdirectory( arinc_615a_api_doc )
add_subdirectory( arinc_615a_internal_doc )
add_subdirectory( arinc_615a_manpage )
add_subdirectory( arinc_615a_user_manual )
# TODO Rework Dependency file generation as PDF output is not available on Debian for the moment, and file generation is not deterministic
#add_subdirectory( arinc_615a_dependencies )
