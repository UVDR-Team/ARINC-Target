# SPDX-License-Identifier: MPL-2.0
# Emit the authoritative production source list for the offline handoff tool.
set(office_sources "")
foreach(library IN ITEMS arinc_665 tftp arinc_615a arinc_615a_tha_target)
  get_target_property(sources ${library} SOURCES)
  foreach(source IN LISTS sources)
    get_filename_component(source "${source}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
    string(APPEND office_sources "${source}\n")
  endforeach()
endforeach()
file(WRITE "${CMAKE_BINARY_DIR}/office-sources.txt" "${office_sources}")
file(WRITE "${CMAKE_BINARY_DIR}/office-boost.txt" "${ARINC_BOOST_INCLUDE_DIR}\n")
