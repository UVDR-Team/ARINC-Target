# SPDX-License-Identifier: MPL-2.0
function(arinc_extract_boost source_root destination)
  set(archive "${source_root}/third_party/boost_1_88_0_headers.tar.gz")
  file(SHA256 "${archive}" actual)
  if(NOT actual STREQUAL "e166e0cdc01b18c9852c59bad215e6e7d1cdb6532543ccec80a3fa43c80387b6")
    message(FATAL_ERROR "Bundled Boost archive is damaged; copy the complete ARINC folder again.")
  endif()
  if(NOT EXISTS "${destination}/boost/version.hpp")
    message(STATUS "Extracting bundled Boost 1.88 headers (offline)")
    file(MAKE_DIRECTORY "${destination}")
    file(ARCHIVE_EXTRACT INPUT "${archive}" DESTINATION "${destination}")
  endif()

  # Boost.Asio 1.88 assumes every non-Windows, non-Symbian platform has
  # sys/poll.h. VxWorks 24.03 uses the select reactor for this target and does
  # not ship that header. Keep the upstream header unchanged in the archive,
  # then apply this deterministic target-only compatibility branch to the
  # extracted offline copy.
  set(socket_types "${destination}/boost/asio/detail/socket_types.hpp")
  file(READ "${socket_types}" socket_types_content)
  set(poll_probe "# elif !defined(__SYMBIAN32__)\n#  include <sys/poll.h>")
  set(vxworks_probe "# elif defined(__VXWORKS__) || defined(__vxworks)\n#  include <selectLib.h>\n# elif !defined(__SYMBIAN32__)\n#  include <sys/poll.h>")
  if(NOT socket_types_content MATCHES "defined\\(__VXWORKS__\\) \\|\\| defined\\(__vxworks\\)")
    string(FIND "${socket_types_content}" "${poll_probe}" poll_probe_position)
    if(poll_probe_position EQUAL -1)
      message(FATAL_ERROR "Bundled Boost.Asio socket_types.hpp has an unexpected layout.")
    endif()
    string(REPLACE "${poll_probe}" "${vxworks_probe}" socket_types_content "${socket_types_content}")
    file(WRITE "${socket_types}" "${socket_types_content}")
  endif()
endfunction()
