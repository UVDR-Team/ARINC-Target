# Bundled dependencies

The target builds without Git, network access, Qt, spdlog, libxml++, an
external Helper library, or a separate ARINC 645/649 library.

* `arinc_665/`: the binary file core and its tests, imported from this
  project's existing local ARINC 665 snapshot; MPL-2.0 license included.
* `boost_1_88_0_headers.tar.gz`: complete Boost 1.88 header tree and license;
  no compiled Boost runtime libraries. CMake verifies the archive and extracts
  it offline. All platform branches are retained, including Windows/VxWorks.
* `lib/arinc_support` and `lib/arinc_checksum`: local sources; required utility
  and CRC/hash functionality is compiled into the project. Historical ARINC
  645 names describe the checksum algorithms, not an external library.

Boost source origin:
https://archives.boost.io/release/1.88.0/source/boost_1_88_0.tar.bz2

Official source SHA256 (verified before creating the header archive):
`46d9d2c06637b219270877c9e16155cbd015b6dc84349af064c088e9b5b12f7b`

Header archive SHA256:
`e166e0cdc01b18c9852c59bad215e6e7d1cdb6532543ccec80a3fa43c80387b6`

Required development platform: licensed Wind River Workbench 4, VxWorks 24.03
VSB/BSP and its LLVM 17.0.6.1 C++ compiler/library in C++17 mode. The target
source no longer requires `std::format`, `std::span`, concepts, or ranges.
These proprietary components are supplied by the office installation and are
not bundled.
The target image also supplies C++ initialization/exceptions/RTTI, POSIX
threads, sockets, clocks, and file-system services used by the application.
