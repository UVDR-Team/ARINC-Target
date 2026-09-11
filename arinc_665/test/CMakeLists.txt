# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

string( RANDOM LENGTH 2048 FILE1_CONTENT )
string( RANDOM LENGTH 4096 FILE2_CONTENT )
string( RANDOM LENGTH 8192 FILE3_CONTENT )
string( RANDOM LENGTH 16384 FILE4_CONTENT )

file( GENERATE OUTPUT File1 CONTENT ${FILE1_CONTENT} )
file( GENERATE OUTPUT File2 CONTENT ${FILE2_CONTENT} )
file( GENERATE OUTPUT File3 CONTENT ${FILE3_CONTENT} )
file( GENERATE OUTPUT File4 CONTENT ${FILE4_CONTENT} )
file( GENERATE OUTPUT Readme.txt CONTENT "Readme Content" )

add_test(
  NAME arinc_665_generate_media_set_rmoutdir_v2
  COMMAND
    ${CMAKE_COMMAND} -E remove_directory MediaSet_V2 )

add_test(
  NAME arinc_665_generate_media_set_v2
  COMMAND
    arinc_665_media_set_compiler
    --xml-file ${CMAKE_CURRENT_SOURCE_DIR}/ExampleMediaSet.xml
    --source-directory ${CMAKE_CURRENT_BINARY_DIR}
    --destination-directory MediaSet_V2
    --create-batch-files All
    --create-load-header-files All )

add_test(
  NAME arinc_665_copy_media_set_manager_config_v2
  COMMAND
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/MediaSetConfig.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/MediaSet_V2/MediaSetConfig.json )

add_test(
  NAME arinc_665_decompile_media_set_v2
  COMMAND
    arinc_665_media_set_decompiler
    --xml-file ${CMAKE_CURRENT_BINARY_DIR}/DecompiledMediaSet_V2.xml
    --source-directory ${CMAKE_CURRENT_BINARY_DIR}/MediaSet_V2/CCC/MEDIUM_001 )


add_test(
  NAME arinc_665_generate_media_set_rmoutdir_v3
  COMMAND
    ${CMAKE_COMMAND} -E remove_directory MediaSet_V3 )

add_test(
  NAME arinc_665_generate_media_set_v3
  COMMAND
    arinc_665_media_set_compiler
    --xml-file ${CMAKE_CURRENT_SOURCE_DIR}/ExampleMediaSet.xml
    --source-directory ${CMAKE_CURRENT_BINARY_DIR}
    --destination-directory MediaSet_V3
    --create-batch-files All
    --create-load-header-files All
    --version Supplement345 )

add_test(
  NAME arinc_665_copy_media_set_manager_config_v3
  COMMAND
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/MediaSetConfig.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/MediaSet_V3/MediaSetConfig.json )

add_test(
  NAME arinc_665_decompile_media_set_v3
  COMMAND
    arinc_665_media_set_decompiler
    --xml-file ${CMAKE_CURRENT_BINARY_DIR}/DecompiledMediaSet_V3.xml
    --source-directory ${CMAKE_CURRENT_BINARY_DIR}/MediaSet_V3/CCC/MEDIUM_001 )
