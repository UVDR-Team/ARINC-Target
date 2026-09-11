# ARINC 615A Target Hardware Application (THA) Suite
### Full Offline Integrated Protocol Stack for VxWorks 26.03 DKM and Desktop
**Maintained by UVDR-Team (uvdrteam@gmail.com)**  
**Target Architecture:** VxWorks 26.03 Downloadable Kernel Module (DKM) / Cross-Platform Linux & Windows  
**Active Development Branch:** `vxworks-complete-port`

---

## 1. Executive Summary & Managerial Alignment

This repository contains the complete, self-contained source distribution of the **ARINC 615A Target Hardware Application (THA)** and its full dependency hierarchy:
- **`arinc_615a`**: ARINC 615A-3 Data Loader Protocol target & host implementation.
- **`app/arinc_615a_test_tha`**: The Target Hardware Application (THA) providing external C and C++ library APIs (`arinc_615a_tha.h`, `Arinc615aThaLib.hpp`) for direct execution from a C-based VxWorks Downloadable Kernel Module (DKM).
- **`helper`**: Low-level foundational utilities (endian conversions, safe casting, raw data spans, async schedulers).
- **`tftp`**: ARINC 615A-compliant TFTP server and client implementation with option negotiation (block size, timeout, transfer size, and custom ARINC options).
- **`arinc-649`**: Checksum and Check Value generation engine (CRC8, CRC16, CRC32, CRC64, MD5, SHA-1/256/512).
- **`arinc_665`**: ARINC 665 Load Header and Batch file binary parsers.
- **`compat`**: Zero-overhead compatibility shims for platforms lacking `<format>` (VxWorks libc++) and `spdlog` (VxWorks kernel).
- **`commands`**: Command registry and CLI dispatch utilities.
- **`qt_icon_resources`**: Embedded icon assets for desktop graphical tools.

### Manager Directive & Engineering Philosophy
> **"We must not merely patch `arinc_615a_test_tha.cpp` in isolation or hack away runtime dependencies ad-hoc. Every dependency and library required by the THA must be physically available, properly audited, and adapted across the entire codebase according to the main source code architecture so that it builds and runs reliably on VxWorks 26.03."**

In accordance with this directive:
1. **Zero External Network Dependencies**: All required sub-projects are included directly within this repository. No Git fetching, submodules, or network calls are triggered during CMake configuration.
2. **Modular Architecture Preserved**: Each component maintains its original identity and modular hierarchy while being cross-compilable for both embedded VxWorks kernel space and desktop environments.
3. **Strict Audit Trail**: Every file modification, compiler guard, compatibility layer, and build flag is strictly documented in this document.

---

## 2. Technical Audit: Previous Failures & Root Cause Analysis

### 2.1 Analysis of the Codex (`ARINC OpenSource`) Attempt
During previous exploratory efforts using Codex, an offline copy was generated in an `ARINC OpenSource` tree. While well-intentioned, it suffered from several critical flaws that prevented compilation on VxWorks:
1. **Monolithic Inlining**: It flattened separate libraries (`helper` -> `lib/arinc_support`, `arinc-649` -> `lib/arinc_checksum`, `tftp` -> `lib/tftp`, `spdlog` -> `lib/spdlog`), destroying the modular structure of the upstream codebase.
2. **Fatal Boost Version Check**: In `cmake/OfflineLibraries.cmake`, Codex hardcoded:
   ```cmake
   if(boost_version LESS 108800)
     message(FATAL_ERROR "Boost >= 1.88 is needed for the original MD5/SHA implementations (Boost.Hash2). Supplied: ${boost_version}.")
   endif()
   ```
   Because VxWorks 7 / 26.03 SDK supplies **Boost 1.86.0** (`BOOST_VERSION 108600`), the configuration failed immediately.
3. **Unresolved Boost.Hash2 Inclusions**: `CheckValueGenerator.cpp` unconditionally included `<boost/hash2/md5.hpp>`, which does not exist in Boost 1.86.0.
4. **Shallow CLI Fixes**: It only addressed the top-level CLI entry point without resolving the deeper library and header requirements.

### 2.2 The 7 Root Causes of VxWorks Build Failures
When `arinc_615a_test_tha.cpp` was compiled directly in Wind River Workbench / VxWorks, the following cascading errors occurred:

| # | Root Cause | Technical Failure Mode in VxWorks | Solution in This Branch |
|---|------------|-----------------------------------|-------------------------|
| **1** | **Missing Export & Version Headers** | `arinc_615a_test_tha.cpp` includes `<arinc_615a/Arinc615a.hpp>`, which requires CMake-generated headers (`arinc_615a_export.h`, `Version.hpp`). Compiling standalone failed immediately. | Build as a structured CMake library target where `generate_export_header` creates all export macros. |
| **2** | **CMake Version Lockout (`VERSION 4.3`)** | Upstream files demanded `cmake_minimum_required(VERSION 4.3)`. The VxWorks SDK ships with **CMake 3.26.4**, triggering a fatal CMake configuration halt. | Standardized all `cmake_minimum_required` calls across all modules to **`VERSION 3.24`**. |
| **3** | **Missing `libxml++-5.0` in `arinc_665`** | `arinc_665` mandated `PkgConfig` and `libxml++-5.0`. VxWorks has no PkgConfig or libxml++ in the kernel. | Guarded `media/` and `utils/` XML code behind `if(NOT ARINC_615A_VXWORKS)`. Compiled only core binary file parsers needed by THA. |
| **4** | **Compiled `boost_program_options` in Kernel** | Boost compiled binaries do not exist in the VxWorks kernel sysroot. Upstream configuration classes depended on `boost::program_options`. | Implemented `ARINC_615A_NO_PROGRAM_OPTIONS` guards and provided direct C/C++ programmatic configuration APIs. |
| **5** | **Missing `<format>` and `spdlog`** | VxWorks Clang 18 libc++ lacks `<format>`, and `spdlog` is not in sysroot. | Created header-only `compat/` layer with zero-overhead `spdlog` logging to `printf`/`logMsg` and `fmt`-backed `<format>`. |
| **6** | **POSIX User Lookups in `helper`** | `KnownDirectoriesLinux.cpp` called `getpwuid_r` and `getuid`, unsupported in VxWorks kernel space. | Added VxWorks platform handling in `helper` build to bypass host user directory database routines. |
| **7** | **Missing Static Archive Linkage** | Compiling `arinc_615a_test_tha.cpp` alone produced hundreds of unresolved symbols from `libarinc_615a.a`, `libarinc_665.a`, `libtftp.a`, `libarinc_649.a`, and `libhelper.a`. | Integrated full dependency tree into root CMake build, compiling each library as an archive and linking into the DKM bundle. |

---

## 3. Architecture & Dependency Graph

The Target Hardware Application sits on top of a fully integrated protocol stack:

```
+-------------------------------------------------------------------------+
|                  VxWorks 26.03 C DKM Application                        |
|                     (Calls arinc_615a_tha.h)                           |
+-------------------------------------------------------------------------+
                                    |
                                    v
+-------------------------------------------------------------------------+
|           arinc_615a_test_tha_lib (Arinc615aThaLib.cpp/hpp)            |
|   - Thread-safe Lifecycle: init, start(background/blocking), stop       |
|   - Target DataLoader Configuration & Operation Handlers                |
+-------------------------------------------------------------------------+
           |                                             |
           v                                             v
+-----------------------------+           +-----------------------------+
|        arinc_615a           |           |          arinc_665          |
|  ARINC 615A Target Protocol |           |  ARINC 665 Binary Parsers   |
|  FIND / INFO / UPLOAD / DL  |           |  LoadHeader & Batch Files   |
+-----------------------------+           +-----------------------------+
      |              |                                   |
      |              +-------------------+               |
      v                                  v               v
+-----------------------------+     +-----------------------------------+
|            tftp             |     |             arinc-649             |
|   TFTP Server & Options     |     |   ARINC 645 Checksum Engine       |
|   Packet Encoding/Decoding  |     |   CRC8 / CRC16 / CRC32 / CRC64    |
+-----------------------------+     +-----------------------------------+
      |                                                  |
      +------------------------+-------------------------+
                               |
                               v
+-------------------------------------------------------------------------+
|                                helper                                   |
|   RawData Spans, SafeCast, Endianness, BCD, Async Schedulers            |
+-------------------------------------------------------------------------+
                               |
                               v
+-------------------------------------------------------------------------+
|                       Kernel Compatibility Layer                        |
|   - compat/spdlog: Lightweight logging to printf / logMsg               |
|   - compat/format: Header-only fmt-backed format shim                   |
|   - Boost Headers (1.86.0 from VxWorks SDK: ASIO, PropertyTree, System) |
+-------------------------------------------------------------------------+
```

---

## 4. Current Implementation Plan for VxWorks 26.03 Port

### Phase 1: Dependency Integration & Build System Consolidation
- [x] Embed all submodules (`helper`, `tftp`, `arinc-649`, `arinc_665`, `commands`, `qt_icon_resources`) directly in the repository.
- [x] Configure root `CMakeLists.txt` to automatically detect internal source folders via `IS_DIRECTORY ${CMAKE_SOURCE_DIR}/<component>`.
- [x] Standardize all `cmake_minimum_required` to `VERSION 3.24` for compatibility with VxWorks SDK CMake 3.26.4.

### Phase 2: VxWorks Kernel Decoupling
- [x] Define `ARINC_615A_VXWORKS` option to toggle DKM static library compilation.
- [x] Exclude desktop GUI (`arinc_615a_qt`, `arinc_615a_dla_qt`, `tftp_qt`, `arinc_665_qt`).
- [x] Exclude desktop XML tools in `arinc_665` (`libxml++-5.0` dependency eliminated).
- [x] Guard all `boost::program_options` behind `ARINC_615A_NO_PROGRAM_OPTIONS`.

### Phase 3: Compatibility Shims & C DKM Wrapper API
- [x] Deploy `compat/spdlog` header shim (printf/logMsg output, no external spdlog library needed).
- [x] Deploy `compat/format` header shim (fmt-based formatting, solving missing `<format>`).
- [x] Implement `app/arinc_615a_test_tha/arinc_615a_tha.h` providing an `extern "C"` API for C DKMs:
  - `arinc615a_tha_init_json(const char *json_str)`
  - `arinc615a_tha_init_file(const char *json_file_path)`
  - `arinc615a_tha_start(int run_in_background)`
  - `arinc615a_tha_stop(void)`
  - `arinc615a_tha_is_running(void)`
  - `arinc615a_tha_set_log_level(int level)`
- [x] Implement `app/arinc_615a_test_tha/Arinc615aThaLib.hpp` and `Arinc615aThaLib.cpp` handling the ASIO event loop lifecycle on a background worker task.

### Phase 4: Checksum & Hash Compatibility for Boost 1.86.0
- [ ] Adapt `arinc-649/lib/arinc_649/CheckValueGenerator.cpp` to use embedded RFC-compliant MD5/SHA implementations or CRC-only fallbacks when Boost < 1.88 is detected.

### Phase 5: Complete Cross-Compilation & Symbol Verification
- [ ] Cross-compile the full stack using `wr-c++ -dkm` and `${WIND_SDK_HOME}/vxsdk/sysroot/mk/dkm.toolchain.cmake`.
- [ ] Link archives: `libarinc_615a_test_tha_lib.a`, `libarinc_615a.a`, `libarinc_665.a`, `libtftp.a`, `libarinc_649.a`, `libhelper.a`.
- [ ] Inspect generated static archives with `wr-nm` to ensure zero undefined references to host or missing symbols.

---

## 5. Exhaustive Changelog & File-by-File Modification Log

Every single modification across the repository is cataloged below:

### 5.1 Root Build Configuration
- `CMakeLists.txt`:
  - Lowered minimum version: `cmake_minimum_required( VERSION 3.24 )`.
  - Added `ARINC_615A_VXWORKS` option (default OFF).
  - Added automated offline resolution for `helper`, `arinc-649`, `arinc_665`, `tftp`, `qt_icon_resources`, `commands`.
  - Added global include directory `compat/` before all other includes.
  - Added compile definitions for VxWorks: `ARINC_615A_NO_PROGRAM_OPTIONS`, `BOOST_ASIO_NO_DEPRECATED`.
  - Enforced `-std=c++17` for VxWorks DKM builds.
- `.gitignore`:
  - Added ignore rules for `build/`, `build-*/`, and `.cache/`.

### 5.2 Compatibility Layer (`compat/`)
- `compat/spdlog/spdlog.h` [NEW]:
  - Provides header-only logging macros (`spdlog::info`, `spdlog::error`, `spdlog::warn`, `spdlog::debug`, `spdlog::trace`).
  - Formats messages and outputs via standard `printf` / `vprintf`.
- `compat/spdlog/common.h` [NEW]:
  - Defines `spdlog::level::level_enum` values (`trace`, `debug`, `info`, `warn`, `err`, `critical`, `off`).
- `compat/format` [NEW]:
  - Header shim exposing `std::format` via embedded `fmt::format`.
- `compat/fmt/` [NEW]:
  - Embedded header-only `fmt` library headers.

### 5.3 ARINC 615A Test THA Application (`app/arinc_615a_test_tha/`)
- `arinc_615a_tha.h` [NEW]:
  - Pure C header exposing initialization, start, stop, and status query functions for integration with C-based VxWorks DKMs.
- `Arinc615aThaLib.hpp` [NEW]:
  - C++ interface declaring `Arinc615aThaLib` class managing configuration, `boost::asio::io_context`, background worker threads, and graceful shutdown.
- `Arinc615aThaLib.cpp` [NEW]:
  - Implementation of THA server runtime, FIND request dispatch, operation routing, error operation completion, and thread-safe stop mechanism.
- `CMakeLists.txt`:
  - When `ARINC_615A_VXWORKS=ON`, compiles `arinc_615a_test_tha_lib` as a static library target containing `Arinc615aThaLib.cpp` and all operation handlers.
  - When building desktop, compiles executable `arinc_615a_test_tha`.
- `arinc_615a_test_tha.cpp`:
  - Refactored main entry point guarded by `#ifndef ARINC_615A_NO_PROGRAM_OPTIONS`.
- `TargetOperatorDefinedDownloadOperation.cpp`:
  - Replaced `<format>` call with portable `std::to_string` for block size formatting.
- `TargetMediaDefinedDownloadOperation.cpp`:
  - Replaced `<format>` call with portable `std::to_string`.

### 5.4 Submodule Build Adjustments
- `helper/CMakeLists.txt`, `helper/lib/helper/CMakeLists.txt`:
  - Lowered minimum CMake version to `3.24`.
  - Guarded `BoostAsioProgramOptions.cpp` and `boost::program_options` behind `ARINC_615A_NO_PROGRAM_OPTIONS`.
  - Set C++17 standard for VxWorks builds.
- `tftp/CMakeLists.txt`, `tftp/lib/tftp/CMakeLists.txt`:
  - Lowered minimum CMake version to `3.24`.
  - Decoupled `boost::program_options` from `tftp` core library.
- `arinc-649/CMakeLists.txt`, `arinc-649/lib/arinc_649/CMakeLists.txt`:
  - Lowered minimum CMake version to `3.24`.
  - Removed unconditional dependency on Boost 1.88 for VxWorks builds.
- `arinc_665/CMakeLists.txt`, `arinc_665/lib/arinc_665/CMakeLists.txt`:
  - Lowered minimum CMake version to `3.24`.
  - Disabled `PkgConfig` and `libxml++-5.0` requirement when `ARINC_615A_VXWORKS=ON`.
  - Excluded `utils/` and `media/` desktop XML modules from VxWorks static library.

---

## 6. Multi-Site Workflow (Office <-> Home Synchronization)

To ensure seamless work between office and home workstations without token collisions or divergence:

### Repository Remote Information
- **Remote URL:** `https://github.com/UVDR-Team/ARINC-Target.git`
- **Primary Development Branch:** `vxworks-complete-port`
- **Authenticated Username:** `UVDR-Team`

### Cloning from a New Machine (Home or Office)
```bash
git clone https://github.com/UVDR-Team/ARINC-Target.git -b vxworks-complete-port
cd ARINC-Target
```

### Configuring Authentication with Personal Access Token
Set the local Git credential helper or configure the remote URL directly:
```bash
# Set your active token for the remote (replace <TOKEN> with your home/office token):
git remote set-url origin https://UVDR-Team:<TOKEN>@github.com/UVDR-Team/ARINC-Target.git
git config user.name "UVDR-Team"
git config user.email "uvdrteam@gmail.com"
```

### Pulling Updates
Before starting work at home or office:
```bash
git fetch origin
git pull origin vxworks-complete-port
```

### Pushing Updates
When pushing updates, ensure this README is updated with all new changes, then commit and push:
```bash
git add .
git commit -m "Your descriptive commit message"
git push origin vxworks-complete-port
```

---

## 7. VxWorks 26.03 DKM Build Instructions

### Step 1: Enter the Wind River Environment
In your Wind River terminal:
```bash
source $WIND_SDK_HOME/sdkenv.sh
wr-c++ --version
```

### Step 2: Configure with CMake
```bash
cmake -S . -B build-vxworks \
      -DCMAKE_TOOLCHAIN_FILE=${WIND_SDK_HOME}/vxsdk/sysroot/mk/dkm.toolchain.cmake \
      -DARINC_615A_VXWORKS=ON \
      -DCMAKE_BUILD_TYPE=Release
```

### Step 3: Build All Static Libraries
```bash
cmake --build build-vxworks -j$(nproc)
```

The resulting archives ready for linking into your DKM project:
```
build-vxworks/app/arinc_615a_test_tha/libarinc_615a_test_tha_lib.a
build-vxworks/lib/arinc_615a/libarinc_615a.a
build-vxworks/arinc_665/lib/arinc_665/libarinc_665.a
build-vxworks/tftp/lib/tftp/libtftp.a
build-vxworks/arinc-649/lib/arinc_649/libarinc_649.a
build-vxworks/helper/lib/helper/libhelper.a
```

### Step 4: C DKM Integration Example
```c
#include "arinc_615a_tha.h"
#include <taskLib.h>

void arinc_tha_task(void)
{
    /* Initialize from JSON configuration file */
    if (arinc615a_tha_init_file("/romfs/arinc_615a_tha.json") != 0) {
        printf("Failed to initialize ARINC 615A THA configuration\n");
        return;
    }

    /* Start loader (blocking in this worker task) */
    printf("Starting ARINC 615A THA Server...\n");
    arinc615a_tha_start(0);
}

void dkm_start(void)
{
    taskSpawn("tArincTha", 100, 0, 65536, (FUNCPTR)arinc_tha_task, 0,0,0,0,0,0,0,0,0,0);
}

void dkm_stop(void)
{
    arinc615a_tha_stop();
}
```

### 5.5 CheckValueGenerator Compatibility Fix (Boost 1.86.0)
- `arinc-649/lib/arinc_649/CheckValueGenerator.cpp`:
  - Added `#include <boost/version.hpp>` and compile guard `#if BOOST_VERSION >= 108800`.
  - Conditioned `<boost/hash2/md5.hpp>`, `sha1.hpp`, `sha2.hpp` behind `ARINC_649_HAVE_BOOST_HASH2`.
  - Enabled all standard ARINC CRC generators (CRC8, CRC16, CRC32, CRC64, NOP) to build cleanly on Boost 1.86.0 (VxWorks SDK version) without missing headers.
- `arinc-649/lib/arinc_649/CMakeLists.txt`:
  - Changed `find_package( Boost 1.88 REQUIRED )` to `find_package( Boost REQUIRED )` to permit Boost 1.86.0 headers from the VxWorks SDK.
