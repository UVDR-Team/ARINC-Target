# Offline ARINC 615A THA for VxWorks 26.03 DKM

This tree builds the ARINC 615A Target Hardware Application (THA) as static
libraries for a C-based VxWorks DKM. It needs no Git access, package manager,
Qt, spdlog, external Helper library, separate ARINC 645 library, or host GUI.
TFTP and the required checksum implementation are local source code.

## 1. Keep the source layout

Keep these two directories beside each other; do not rename either directory:

```text
ARINC/
  arinc_615a-main/       <- this repository and build entry point
  arinc_665-main/        <- local ARINC 665 source tree
```

Build from `arinc_615a-main`. The default `ARINC665_SOURCE_DIR` already points
to `../arinc_665-main`.

## 2. Prepare the VxWorks project

In Workbench 4, create a DKM project for the same VxWorks 26.03 image and BSP
that will run the loader. Enable the kernel facilities needed by the protocol:

- C++ runtime and C++23 support;
- IPv4, UDP sockets, and loopback/network interface support;
- POSIX file-system support if the JSON file, upload directory, or download
  directories are stored on the target file system;
- the Boost headers supplied by the VxWorks SDK, version 1.88 or newer.

The Boost directory passed below must directly contain `boost/version.hpp`.
For example, if the file is `<sdk>/boost_1_88_0/boost/version.hpp`, use
`<sdk>/boost_1_88_0` as `ARINC_BOOST_INCLUDE_DIR`.

## 3. Enter the Wind River build environment

Run these commands in a Wind River command shell, not a normal macOS shell.
The profile name can differ in your installation; select the profile shown by
your Workbench/VxWorks 26.03 installation.

```sh
wrenv -p vxworks-26.03
wr-c++ --version
```

`wr-c++` must report the compiler used for the destination's CPU/ABI. Do not
mix libraries built with a host compiler and a VxWorks DKM compiler.

## 4. Create the CMake toolchain file

Create `vxworks-dkm-toolchain.cmake` outside the source tree. Replace the CPU
value with the DKM destination architecture (for example `aarch64`, `arm`, or
`x86_64`) and add your BSP-specific compiler flags only if Workbench requires
them.

```cmake
set(CMAKE_SYSTEM_NAME VxWorks)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER wr-cc)
set(CMAKE_CXX_COMPILER wr-c++)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

## 5. Configure and build offline

Use CMake and Ninja supplied by Workbench, or an offline-installed equivalent.
Replace the two placeholder paths. All desktop-only features must stay OFF.

```sh
cmake -S "/absolute/path/ARINC/arinc_615a-main" \
      -B "/absolute/path/build-vxworks" \
      -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="/absolute/path/vxworks-dkm-toolchain.cmake" \
      -DARINC_BOOST_INCLUDE_DIR="/absolute/path/boost_1_88_0" \
      -DARINC_ENABLE_COMMAND_LINE=OFF \
      -DARINC615A_THA_BUILD_TEST_APPLICATION=OFF \
      -DARINC_BUILD_TESTS=OFF

cmake --build "/absolute/path/build-vxworks" --target arinc_615a_tha_target
```

The build must not download anything. A failed Boost version check means the
VxWorks SDK Boost is older than 1.88; use matching offline Boost 1.88+ headers
for the same VxWorks environment.

## 6. Add the libraries to the DKM

Add the build directory to the DKM library search path and link these archives
in this order:

```text
libarinc_615a_tha_target.a
libarinc_615a.a
libarinc_665.a
libtftp.a
```

`arinc_checksum` is compiled into `libarinc_665.a`; do not add ARINC 645.
There is no Helper or spdlog archive to add. Also link the normal VxWorks C++
and networking libraries selected by the DKM project.

Add this include directory to the DKM project:

```text
<ARINC>/arinc_615a-main/app/arinc_615a_unit_test/arinc_615a_test_tha
```

## 7. Start and stop from C

Include the C boundary header in the DKM source:

```c
#include "Arinc615aThaC.h"
```

Start the loader from a dedicated task, never from the DKM initialization
routine, because the call blocks while it runs the ASIO event loop:

```c
void arincThaWorker(void)
{
    (void)arinc615a_tha_run_file("/romfs/arinc_615a_tha.json");
}
```

Create the worker using your project's approved VxWorks task creation method.
To stop it during DKM unload or controlled shutdown:

```c
if (arinc615a_tha_request_stop() == 0) {
    /* Loader was not running, or its event loop had already stopped. */
}
```

Wait for the worker task to exit before unloading the DKM. Never unload a DKM
while its THA task is executing.

## 8. Deploy and edit the configuration

Start with:

```text
app/arinc_615a_unit_test/arinc_615a_test_tha/arinc_615a_test_tha.json.template
```

Copy it to the target file system as `/romfs/arinc_615a_tha.json` (or pass your
chosen full path to `arinc615a_tha_run_file`). Set the target interface IP,
FIND port, target IDs, enabled operations, status rate, and upload/download
directories for the actual unit. Every referenced directory must exist and be
writable/readable as appropriate before starting the loader.

## 9. Acceptance checks

1. Load the DKM; confirm no unresolved symbols are reported.
2. Start the worker task; confirm it remains running.
3. Send an ARINC 615A FIND request and verify the configured FIND response.
4. Exercise each enabled Information, Upload, Media Defined Download, and
   Operator Defined Download operation with a ground loader.
5. Verify invalid target IDs and disabled operations are rejected.
6. Call `arinc615a_tha_request_stop`, wait for task exit, then unload the DKM.
7. Repeat the load/start/stop/unload cycle at least three times.

## What is intentionally excluded

The DKM profile excludes host command-line parsing, Boost.Program_options,
GUI/Qt code, documentation targets, Git metadata, network downloads, external
Helper, spdlog, and a separate ARINC 645 target. Boost remains a kernel/SDK
header dependency because upstream checksum code requires Boost.Hash2.
