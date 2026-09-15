# Wind River Workbench / VxWorks DKM integration

This profile replaces the former `ARINC615A_DKM_PROFILE` / prebuilt-dependency setup. The current switches are `ARINC615A_TARGET_ONLY=ON` and `ARINC_615A_VXWORKS=ON`.

## 1. Select the DKM SDK

Use the same SDK, CPU/BSP, C++ runtime and ABI as the destination kernel image. Enter its Wind River environment before building. The toolchain must target a Downloadable Kernel Module, not an RTP.

No CPU or BSP is hard-coded here. The repository records VxWorks 26.03 and Boost 1.86 as the office setup; the actual office SDK was not available during this audit.

Requirements:

- CMake 3.24 or newer.
- DKM C/C++ compiler and archiver selected by the SDK toolchain.
- C++20 support including concepts, span, ranges, bind_front and filesystem; C++ exceptions and RTTI must be enabled.
- SDK Boost headers version 1.86 or newer. No compiled Boost libraries.
- Target image support for UDP sockets, timers, synchronization and file I/O.

CMake-generated export and version headers are created in `build-vxworks/generated`. They must not be hand-written or copied from a different build.

## 2. Build

For an SDK shell with Bash, use:

```sh
export ARINC615A_TOOLCHAIN="/absolute/path/to/dkm.toolchain.cmake"
export ARINC615A_BOOST_INCLUDE_DIR="/absolute/path/containing/boost"
bash tools/build_vxworks.sh
```

For Workbench's custom/external build command, invoke that script from the project directory inside the selected SDK environment. Alternatively, use these cross-platform CMake commands from the Wind River terminal:

```sh
cmake -S . -B build-vxworks \
  -DCMAKE_TOOLCHAIN_FILE="/absolute/path/to/dkm.toolchain.cmake" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DARINC615A_BOOST_INCLUDE_DIR="/absolute/path/containing/boost" \
  -DARINC615A_TARGET_ONLY=ON -DARINC_615A_VXWORKS=ON \
  -DARINC615A_BACKGROUND_THREAD=OFF -DARINC615A_BUILD_TESTS=OFF \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-vxworks --parallel 4
cmake --build build-vxworks --target arinc615a_audit
cmake --install build-vxworks --prefix build-vxworks/stage
```

Shell continuation syntax differs on Windows; the same CMake options can be entered on one line. Use a fresh build directory when changing toolchains/SDKs.

The build contains no FetchContent, network download, Qt, XML, pkg-config or compiled Boost discovery. `cmake/TargetSources.cmake` is the reviewed source manifest; adding a source requires an explicit manifest change. Kernel macro cleanup is applied to all C++ translation units by `compat/vxworks_platform.hpp`.

## 3. Link the DKM application

Add `stage/include` to your C application's header search path. Add these archives from `stage/lib` to the DKM link:

```text
libarinc_615a_test_tha_lib.a
libarinc_615a.a
libarinc_665.a
libtftp.a
libarinc_649.a
libhelper.a
```

Names use the SDK's static library prefix/suffix. This list is dependency order for a conventional archive linker. Use the SDK/Workbench C++ DKM link process so C++ static initialization and the matching C++ runtime are included. Do not link native Mac/Linux validation archives into the target.

Static archives may contain unresolved references to each other and to the kernel C/C++ runtime. Those must resolve in the final DKM link/load. `arinc615a_audit` rejects known desktop dependencies; it cannot prove the contents of an unavailable board image.

Do not compile `app/arinc_615a_test_tha/arinc_615a_test_tha.cpp` as the DKM entry point: it is the old desktop CLI. Use the C interface in `arinc_615a_tha.h` and the example in `examples/vxworks_entry.c`.

## 4. Configure and run

Copy the JSON template to a readable location on the target. Set the actual target IDs, interfaces/ports and existing writable upload/download directories. The example template contains desktop paths and sample identities; update them for the board.

The worker task calls `arinc615aDkmRun(configuration_path)` in the example. Initialization reports invalid JSON and unsupported configured hash algorithms as errors. A failed initialization leaves the previous configuration intact. Reconfiguration while the service is running is rejected.

The worker blocks in `arinc615a_tha_start(0)`. Another task requests stop. Stop waits for event callbacks to exit before resource destruction. Before unloading, also wait for the application-owned worker task to return. Do not use task deletion as the stop mechanism.

Background start returns `ARINC615A_UNSUPPORTED` unless `ARINC615A_BACKGROUND_THREAD=ON` was explicitly built with an SDK/runtime that supports C++ thread creation.

On Boost 1.86, configure `NotUsed`, `CRC8`, `CRC16`, `CRC32` or `CRC64` as appropriate. MD5/SHA require Boost >= 1.88 and are rejected at configuration time on 1.86; they never silently become an unchecked CRC substitute.

## 5. Verification and office troubleshooting

Native verification covers all selected translation units, a forced whole-archive executable link, existing binary protocol tests, and UDP FIND/upload-initialization plus lifecycle regressions. The Boost 1.86 build is also exercised with address and undefined-behavior sanitizers. See `DEPENDENCY_AUDIT.md` for the recorded results.

The final Workbench cross-compile, DKM link/load and loader-to-board transfers remain SDK/board validation steps. If the office build fails, collect:

- The first compiler/linker error and its full command line.
- `build-vxworks/BUILD_INFO.txt`.
- The generated `CMakeCache.txt` and `compile_commands.json`.
- `build.log`, `audit.log`, and the selected SDK/compiler version.

These identify the actual CPU/BSP, toolchain and remaining platform services without guessing them.
