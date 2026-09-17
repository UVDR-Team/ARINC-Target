# ARINC 615A VxWorks target stack

This branch contains an audited, offline, target-only build of the ARINC 615A Target Hardware Application (THA). It is intended for a Wind River Workbench **Downloadable Kernel Module (DKM)** project.

The target build compiles only the protocol code needed on the board. Desktop applications and their dependency graph are still retained in the full repository for history, but they are not configured or compiled by the default build.

> With no Internet access on the office PC, follow [OFFLINE_VXWORKS_WORKBENCH.md](OFFLINE_VXWORKS_WORKBENCH.md) from source transfer through board validation. For deeper integration details, see [VxWorksDKM.md](VxWorksDKM.md). The completed source/dependency review is recorded in [DEPENDENCY_AUDIT.md](DEPENDENCY_AUDIT.md).

## What this branch fixes

The office branch still pulled desktop code and dependencies into the DKM build and contained several portability and lifecycle defects. This cleanup:

- replaces the repository-wide build graph with an explicit target manifest;
- removes target dependencies on Qt, XML, command utilities, pkg-config and compiled Boost libraries;
- removes target uses of Boost.Program_options;
- brings target library code down from accidental C++23 usage to C++20;
- provides portable formatting and underlying-enum helpers;
- fixes unaligned endian access and the endian description API mismatch;
- fixes upload-directory and FIND-request validation;
- rejects unavailable Boost 1.86 hash algorithms during initialization;
- makes THA start, stop and restart safe across application-owned tasks;
- retains asynchronous error operations until completion;
- adds dependency auditing, whole-archive link validation, lifecycle tests and a minimal source packager.

## Target contents

The reviewed manifest is [cmake/TargetSources.cmake](cmake/TargetSources.cmake). It currently contains 135 C++ translation units and creates these six archives:

| Archive | Responsibility |
| --- | --- |
| `libhelper.a` | Raw binary data, endian conversion, descriptions and portable utilities |
| `libarinc_649.a` | CRC/check-value generation |
| `libarinc_665.a` | ARINC 665 binary file parsing and encoding |
| `libtftp.a` | TFTP client/server protocol used by ARINC 615A |
| `libarinc_615a.a` | FIND server, protocol files and target operations |
| `libarinc_615a_test_tha_lib.a` | THA configuration, operation handlers and C API |

The build also uses the repository's header-only fmt/spdlog compatibility layer and Boost headers supplied by the SDK.

The following are deliberately excluded from the target graph:

- Qt, icons and GUI applications;
- command registry and desktop command-line programs;
- ARINC 615A host operations and FIND client;
- ARINC 665 XML/media compiler, copier and printer tools;
- libxml++, pkg-config and all network downloads;
- Boost.Program_options, Boost.Filesystem and Boost.Thread binaries;
- the helper scheduler and desktop home-directory lookup code;
- native tests and the old standalone desktop `main()`.

## Requirements

Use the same Wind River SDK, CPU/BSP, ABI and C++ runtime as the destination VxWorks image.

Required:

- a VxWorks DKM C/C++ toolchain file;
- CMake 3.24 or newer;
- C++20 compiler and standard library support;
- exceptions and RTTI enabled;
- Boost headers 1.86 or newer;
- kernel/runtime support for UDP sockets, timers, synchronization, filesystem/file I/O and the C++ runtime.

No compiled Boost library is required. The directory assigned to `ARINC615A_BOOST_INCLUDE_DIR` must directly contain `boost/version.hpp`.

No CPU or BSP is hard-coded. That information comes from the selected Wind River toolchain, preventing this repository from silently building for the wrong board.

## Office quick start

### 1. Get the reviewed branch

From a terminal:

```sh
git clone https://github.com/UVDR-Team/ARINC-Target.git
cd ARINC-Target
git fetch origin
git checkout fix/vxworks-dependency-audit
git pull --ff-only
```

If the repository already exists:

```sh
git fetch origin
git switch fix/vxworks-dependency-audit
git pull --ff-only
git status
```

`git status` should report that the branch is up to date and the working tree is clean.

### 2. Enter the Wind River environment

Open a Wind River/VxWorks development shell for the exact SDK and board project used by Workbench. Confirm that the compiler and SDK environment are active before running CMake.

Set these two values:

```sh
export ARINC615A_TOOLCHAIN="/absolute/path/to/the/sdk/dkm.toolchain.cmake"
export ARINC615A_BOOST_INCLUDE_DIR="/absolute/path/whose/boost/version.hpp/exists"
```

If `WIND_SDK_HOME` is set and contains exactly one `boost/version.hpp`, the build script can discover Boost automatically:

```sh
export WIND_SDK_HOME="/absolute/path/to/the/sdk"
export ARINC615A_TOOLCHAIN="$WIND_SDK_HOME/vxsdk/sysroot/mk/dkm.toolchain.cmake"
bash tools/build_vxworks.sh
```

If that toolchain path is different in your installation, locate the DKM `.cmake` toolchain supplied with the SDK and set `ARINC615A_TOOLCHAIN` explicitly. Do not use the host Windows/Linux/macOS compiler.

### 3. Build and audit

The recommended command is:

```sh
bash tools/build_vxworks.sh
```

It performs configuration, compilation, dependency audit and installation. The build is offline and does not download dependencies.

Equivalent manual commands:

```sh
cmake -S . -B build-vxworks \
  -DCMAKE_TOOLCHAIN_FILE="$ARINC615A_TOOLCHAIN" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DARINC615A_BOOST_INCLUDE_DIR="$ARINC615A_BOOST_INCLUDE_DIR" \
  -DARINC615A_TARGET_ONLY=ON \
  -DARINC_615A_VXWORKS=ON \
  -DARINC615A_BACKGROUND_THREAD=OFF \
  -DARINC615A_BUILD_TESTS=OFF \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-vxworks --parallel 4
cmake --build build-vxworks --target arinc615a_audit
cmake --install build-vxworks --prefix build-vxworks/stage
```

Use a new build directory whenever the SDK, CPU/BSP or toolchain changes. A ready-to-edit CMake cache example is provided in [VxWorksDKMDependencies.cmake.example](VxWorksDKMDependencies.cmake.example).

### 4. Check the output

After a successful run, `build-vxworks/stage` contains:

```text
stage/
├── include/
│   └── arinc_615a_tha.h
├── lib/
│   ├── libarinc_615a_test_tha_lib.a
│   ├── libarinc_615a.a
│   ├── libarinc_665.a
│   ├── libtftp.a
│   ├── libarinc_649.a
│   └── libhelper.a
└── share/arinc615a/
    ├── arinc_615a_test_tha.json.template
    ├── BUILD_INFO.txt
    ├── DEPENDENCY_AUDIT.txt
    └── LICENSE
```

Also retain:

- `build-vxworks/build.log`;
- `build-vxworks/audit.log`;
- `build-vxworks/compile_commands.json`;
- `build-vxworks/CMakeCache.txt`.

These files identify the actual compiler, CPU/BSP/toolchain choices and are the first items to collect if Workbench reports a platform-specific error.

## Wind River Workbench integration

Use the repository as a CMake/external-build project, or point an existing Workbench DKM application's external build step at `tools/build_vxworks.sh`.

Important rules:

1. Select a **DKM** toolchain, not an RTP toolchain.
2. Do not configure Workbench to compile every `.cpp` file in the repository. CMake's explicit target manifest is the build boundary.
3. Do not compile `app/arinc_615a_test_tha/arinc_615a_test_tha.cpp`; it is the old desktop CLI entry point.
4. Add `build-vxworks/stage/include` to the application include path.
5. Link the DKM through the SDK's C++ linker flow so static initialization and the matching C++ runtime are included.
6. Never link the native macOS/Linux test archives into a VxWorks module.

For a traditional one-pass archive linker, add the libraries in this dependency order:

```text
libarinc_615a_test_tha_lib.a
libarinc_615a.a
libarinc_665.a
libtftp.a
libarinc_649.a
libhelper.a
```

If Workbench supports archive groups/whole-archive semantics, they may be used according to the SDK linker documentation. Individual static archives normally contain unresolved cross-library and runtime references; those must resolve at the final DKM link.

## Runtime integration

The public C-compatible interface is [app/arinc_615a_test_tha/arinc_615a_tha.h](app/arinc_615a_test_tha/arinc_615a_tha.h). A board integration skeleton is in [examples/vxworks_entry.c](examples/vxworks_entry.c).

Normal task lifecycle:

```c
if (arinc615a_tha_init_file("/path/on/target/arinc615a.json") != ARINC615A_OK) {
    /* report configuration failure */
    return;
}

/* This blocks the current application-owned VxWorks task. */
(void)arinc615a_tha_start(0);
```

A different application task stops the service:

```c
arinc615a_tha_stop();
```

Before unloading the DKM:

1. call `arinc615a_tha_stop()` from a task other than the blocking start task;
2. wait for `arinc615a_tha_stop()` to return;
3. wait for the application-owned task running `arinc615a_tha_start(0)` to return;
4. only then unload the module.

Do not delete the worker task as a substitute for stopping the service.

The DKM profile defaults to `ARINC615A_BACKGROUND_THREAD=OFF`. Therefore `arinc615a_tha_start(1)` returns `ARINC615A_UNSUPPORTED`. This avoids assuming that the BSP permits arbitrary `std::thread` creation.

Return codes:

| Code | Meaning |
| --- | --- |
| `ARINC615A_OK` | Operation completed successfully |
| `ARINC615A_ERROR` | Invalid input, initialization or runtime failure |
| `ARINC615A_ALREADY_RUN` | Duplicate start or live reconfiguration request |
| `ARINC615A_UNSUPPORTED` | Requested feature was disabled for this target build |

## Target configuration

Start with `app/arinc_615a_test_tha/arinc_615a_test_tha.json.template` and copy it to a filesystem location readable by the target.

Before running, review every value:

- target identifiers;
- network interfaces and UDP/TFTP ports;
- upload and download directories;
- permissions and available storage;
- check-value/hash selection.

All configured directories must already exist on the target and have the required read/write permissions. Sample desktop paths in the template are not valid board paths.

With Boost 1.86, use `NotUsed`, `CRC8`, `CRC16`, `CRC32` or `CRC64` as appropriate. MD5, SHA1, SHA256 and SHA512 require Boost 1.88 or newer. On Boost 1.86 they are rejected during initialization instead of failing later during a transfer.

## Validation already completed

The cleanup was tested with AppleClang 17 and official Boost 1.86 headers:

- all 135 selected target translation units compiled;
- all six archives were forced into a final executable link;
- 238 protocol regression cases passed;
- UDP FIND/TFTP and lifecycle tests passed;
- repeated initialization, duplicate start, stop/restart and occupied-port recovery passed;
- invalid targets and existing upload-directory initialization were exercised;
- stopping with pending transfers/timers passed;
- AddressSanitizer and UndefinedBehaviorSanitizer checks passed;
- isolated minimal-package compilation and dependency audit passed;
- static audit found no target references to Qt, XML, commands, Boost.Program_options, Boost.Filesystem or Boost.Thread.

Run native regression tests only on a development host:

```sh
cmake -S . -B build-native \
  -DARINC615A_BOOST_INCLUDE_DIR="/path/to/boost" \
  -DARINC615A_BUILD_TESTS=ON \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build-native --parallel 4
ctest --test-dir build-native --output-on-failure
cmake --build build-native --target arinc615a_audit
```

The upstream test-data generator uses C++23, but it is test-only. All target libraries use C++20.

## Minimal source-only package

To create a package that contains only the reviewed target sources and their recursively resolved local headers:

```sh
python3 tools/package_target.py
```

Output:

```text
dist/arinc615a-target-source.zip
```

The generated ZIP contains the standalone target CMake project, 135-source manifest, required headers/templates, compatibility layer, licenses, build script and documentation. It does not contain the Boost installation, SDK, native test sources or build products.

The ZIP and all `build-*` directories are reproducible generated artifacts and are intentionally ignored by Git. The generator, explicit source manifest and every source needed to recreate them are tracked and pushed.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| `ARINC615A_BOOST_INCLUDE_DIR must contain boost/version.hpp` | Point the variable at the parent directory of the `boost` folder |
| Boost version rejected | Use SDK Boost 1.86 or newer |
| Host compiler appears in `BUILD_INFO.txt` | The DKM toolchain was not selected; delete the build directory and reconfigure from the Wind River environment |
| C++20 type or library symbol missing | Confirm the selected SDK compiler/runtime supports C++20 and that Workbench did not override the standard |
| Exception/RTTI link errors | Enable exceptions and RTTI in the DKM C++ project and use the matching SDK runtime |
| Missing socket/timer/filesystem symbols | Confirm those components are included in the VxWorks image/BSP and use the SDK's DKM link rules |
| Duplicate symbols or desktop `main()` | Do not compile repository sources outside the explicit CMake targets |
| Missing ARINC symbols at final link | Preserve the documented archive order or use the SDK's archive-group feature |
| Service fails during initialization | Validate JSON, target IDs, directory existence/permissions and Boost 1.86 hash restrictions |
| Service does not unload safely | Stop from another task and wait for both stop and the blocking worker task to finish |
| Audit reports a forbidden dependency | Inspect `DEPENDENCY_AUDIT.txt`, dependency output and `compile_commands.json` before adding any library |

When reporting an office failure, provide the **first** compiler/linker error (not only the last cascading errors), its complete command line, `BUILD_INFO.txt`, `CMakeCache.txt`, `compile_commands.json`, `build.log`, and `audit.log`.

## Repository completeness

The Git branch contains all authored source, CMake files, tests, compatibility headers, scripts, examples, templates, licenses and documentation required to reproduce this audit and target package. Build directories, generated archives/object files, IDE metadata, `.DS_Store` and `dist/*.zip` are intentionally excluded because they are machine-specific or reproducible outputs.

The final Wind River compiler/link/load test still has to be performed with the actual office SDK and selected board image. Native validation cannot honestly certify an unavailable CPU/BSP, but the build records the platform information needed to resolve any remaining SDK-specific issue.
