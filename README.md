# ARINC 615A target stack

The default build produces six static libraries and a C API for a target hardware application. It uses the dependency sources already in this repository and Boost headers from your SDK. It does not configure or fetch desktop dependencies.

Start with [VxWorksDKM.md](VxWorksDKM.md) for Wind River Workbench integration. [DEPENDENCY_AUDIT.md](DEPENDENCY_AUDIT.md) records the audit, changes, validation and remaining platform checks.

## Target build

Enter the environment for your selected VxWorks **DKM** SDK, then run:

```sh
export ARINC615A_TOOLCHAIN="/path/to/sdk/dkm.toolchain.cmake"
export ARINC615A_BOOST_INCLUDE_DIR="/path/to/sdk/include"
bash tools/build_vxworks.sh
```

The Boost directory must contain `boost/version.hpp` and must be version 1.86 or newer. The script can find it when `WIND_SDK_HOME` contains exactly one candidate. The selected SDK toolchain supplies the CPU, BSP/sysroot and kernel compilation flags.

Outputs are in `build-vxworks/stage`: six archives, `arinc_615a_tha.h`, configuration template, and build information. Link all six archives into your application's C++-linked DKM; do not compile the old standalone desktop application entry point.

## Included and excluded dependencies

| Included in the target | Purpose |
| --- | --- |
| helper | Binary data, descriptions and portable utilities |
| arinc_649 | CRC8/16/32/64; optional Boost.Hash2 algorithms on Boost >= 1.88 |
| arinc_665 | Binary file parsing and encoding, without XML/media tooling |
| tftp | Client and server transfers used by the target |
| arinc_615a | FIND server, protocol files and target operations |
| arinc_615a_test_tha_lib | Configuration, operation handlers and C API |
| Bundled fmt and logging adapter | Header-only formatting and printf diagnostics |

Excluded: Qt and icons, GUI and command-line executables, command registry, ARINC 615A host operations/FIND client, ARINC 665 XML/media tools, libxml++, pkg-config discovery, compiled Boost libraries, helper scheduler and host home-directory lookup code.

Desktop source directories remain in the full repository for history and independent development. They are absent from the target build and the minimal source package. The old desktop build is opt-in with `ARINC615A_TARGET_ONLY=OFF` and is outside this validation.

## Runtime interface

Use `arinc615a_tha_init_file()` or `arinc615a_tha_init_json()`, then call `arinc615a_tha_start(0)` from an application-owned worker task. Stop from another task using `arinc615a_tha_stop()`. Wait for the worker task to return before unloading the DKM. The DKM profile does not create a C++ background thread by default.

With Boost 1.86, MD5/SHA configurations are rejected explicitly; CRC algorithms remain available. The code still requires the SDK's C++20 standard library, exceptions, filesystem/file I/O, synchronization, UDP sockets and timers. These are target runtime services, not removable desktop libraries.

## Native verification

```sh
cmake -S . -B build-native -DARINC615A_BOOST_INCLUDE_DIR=/path/to/boost \
  -DARINC615A_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-native --parallel 4
ctest --test-dir build-native --output-on-failure
cmake --build build-native --target arinc615a_audit
```

The libraries compile as C++20. Existing upstream test-data generators use C++23 and are never part of the DKM build. Tests include a forced whole-archive link of all six libraries and local UDP integration tests.

## Minimal source package

```sh
python3 tools/package_target.py
```

This creates `dist/arinc615a-target-source.zip` from the explicit translation-unit manifest and the recursively resolved local headers. It includes the required source, generated-header templates, bundled formatting headers, licenses, build script and integration instructions. It contains no desktop projects, native build artifacts or Boost installation.

Native compilation is verified separately from actual Wind River compilation. A successful native build does not establish that a particular board image supplies every required SDK/runtime symbol.
