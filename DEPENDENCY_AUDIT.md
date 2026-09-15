# ARINC 615A target dependency audit

Audit base: office branch commit `febdbba62bf609085fe587975c65206207b31cf8`.

## Target build boundary

The DKM profile compiles an explicit manifest of 135 C++ source files into six static archives: `helper`, `arinc_649`, `arinc_665`, `tftp`, `arinc_615a`, and `arinc_615a_test_tha_lib`. It does not glob the repository.

The profile excludes all desktop-only graph elements: Qt and icon resources, command-line applications and command registry, ARINC 615A host operations and FIND client, ARINC 665 XML/media utilities, libxml++, pkg-config, compiled Boost libraries, `Boost.Program_options`, and network dependency downloads.

The remaining third-party inputs are header-only Boost 1.86 or newer and the bundled header-only fmt/spdlog compatibility headers. The build uses C++20. Boost 1.86 does not provide Boost.Hash2, so MD5/SHA1/SHA256/SHA512 selections are rejected during configuration instead of failing later; CRC algorithms remain available. Boost 1.88 or newer enables those hashes.

## Correctness and portability changes

- Replaced C++23-only library uses (`std::to_underlying`, `std::bind_back`, integer size suffixes, and `string_view::contains`) with C++20 equivalents.
- Replaced direct `std::format` dependency with the target formatting adapter.
- Removed all target uses of Boost.Program_options.
- Made endian loads/stores safe for unaligned buffers and repaired the endian description API mismatch.
- Fixed upload-directory validation and FIND request validation.
- Reworked THA start/stop/restart ownership so servers and the I/O context are not destroyed while a worker is running.
- Preserved overlapping error operations until their asynchronous completion and guarded unsupported hash generators.
- Added a C entry-point example for an application-owned VxWorks task.

## Automated verification

Completed on macOS with AppleClang 17 and the official Boost 1.86 headers:

- All 135 target translation units compiled in both background-worker and DKM-style no-background-worker configurations.
- Protocol regression suite passed (238 Boost.Test cases).
- Forced whole-archive link passed, proving every object in all six archives participates in a final executable link.
- UDP FIND/TFTP lifecycle tests passed, including repeated initialization, duplicate start rejection, stop/restart, occupied ports, invalid target requests, and stop with pending transfers.
- AddressSanitizer and UndefinedBehaviorSanitizer configuration passed the same link and lifecycle checks.
- Static dependency audit found no compiled references or headers from Boost.Program_options, Boost.Filesystem, Boost.Thread, Qt, XML, or the command framework.

## SDK boundary

This machine does not contain the Wind River VxWorks 26.03 compiler, selected CPU/BSP sysroot, or Workbench linker. Therefore the final SDK compile and DKM link must be run in the office environment. `tools/build_vxworks.sh` deliberately takes its compiler, ABI, kernel headers, C++ runtime, socket APIs, and linker policy from that SDK and records them in `BUILD_INFO.txt`. A successful native audit is strong source-level evidence, but it is not represented as an actual BSP certification.
