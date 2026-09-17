# Offline VxWorks / Workbench step-by-step guide

This guide starts with the reviewed `fix/vxworks-dependency-audit` source and ends with an ARINC 615A Target Hardware Application (THA) running on a VxWorks board. It assumes the office PC has no Internet access.

The repository produces six target static libraries. The office Wind River SDK must perform the final CPU/BSP-specific compile, DKM link, load, and board test. Do not use archives built on macOS or Linux on the board.

## 0. Record the board details first

Before building, write down:

- VxWorks release: expected office setup is 26.03, but use the installed release;
- board and BSP name;
- CPU architecture and ABI/endian mode;
- kernel image/VIP name;
- Wind River SDK/compiler version;
- target IP address, subnet mask, and host PC IP address;
- target filesystem path that will hold the JSON and transfer directories.

The DKM must use the same SDK, CPU/BSP, ABI, C++ runtime, exception/RTTI settings, and kernel image as the board.

## 1. Prepare the offline bundle on this Mac

The working source is:

```text
arinc_615a-vxworks-cleanup
```

It should be on branch `fix/vxworks-dependency-audit`. To verify without using the network:

```sh
git status --short --branch
git log -1 --oneline
```

Choose either transfer method.

### Method A: copy the repository directory

Copy the complete `arinc_615a-vxworks-cleanup` directory to an approved USB drive or other approved offline media. Do not copy an existing `build-native` or `build-vxworks` directory; those outputs are machine/toolchain-specific.

### Method B: create the minimal source ZIP

From the repository root:

```sh
python3 tools/package_target.py
```

Copy `dist/arinc615a-target-source.zip` to the approved media. This ZIP contains the reviewed target source but intentionally does not contain the Wind River SDK or Boost.

Also copy this guide. If Boost is not already present inside the office SDK, separately copy an approved Boost header tree version 1.86 or newer. The required directory is the parent of `boost/version.hpp`; no compiled Boost library is needed.

## 2. Check the offline office PC

Copy or extract the source to a short local path without unusual characters. Examples:

```text
C:\work\arinc615a
/work/arinc615a
```

Confirm that these items are available locally:

1. Wind River Workbench and the SDK for the actual BSP.
2. A DKM C and C++ compiler/toolchain, not an RTP-only toolchain.
3. CMake 3.24 or newer (`cmake --version`).
4. Boost headers 1.86 or newer (`boost/version.hpp`).
5. A VxWorks image with C++ runtime, UDP sockets, timers, synchronization, filesystem, and file-I/O support.

No step in this guide requires `git fetch`, `git pull`, `pip`, a package manager, or a web download.

## 3. Open the correct Wind River environment

Start the Wind River development shell associated with the same SDK/BSP used by the Workbench project. Do not use an ordinary Windows, Linux, or macOS compiler shell.

Locate the SDK's DKM CMake toolchain file. A common SDK layout is:

```text
<WIND_SDK_HOME>/vxsdk/sysroot/mk/dkm.toolchain.cmake
```

Installations differ, so use the DKM toolchain shipped with the selected SDK if the path is different.

Set these values in a Bash-compatible Wind River shell:

```sh
export ARINC615A_TOOLCHAIN="/absolute/path/to/dkm.toolchain.cmake"
export ARINC615A_BOOST_INCLUDE_DIR="/absolute/path/containing/boost"
```

Verify both inputs:

```sh
test -f "$ARINC615A_TOOLCHAIN" && echo "DKM toolchain found"
test -f "$ARINC615A_BOOST_INCLUDE_DIR/boost/version.hpp" && echo "Boost headers found"
```

If the Wind River terminal is not Bash-compatible, skip the environment variables and use the one-line CMake command in step 4 with literal absolute paths.

## 4. Cross-build the six libraries offline

### Recommended Bash command

From the repository root:

```sh
bash tools/build_vxworks.sh
```

The script configures, builds, audits, and installs locally. It does not contact the network.

### Manual command

Use this if the script cannot run in the office shell:

```sh
cmake -S . -B build-vxworks -DCMAKE_TOOLCHAIN_FILE="/absolute/path/to/dkm.toolchain.cmake" -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY -DARINC615A_BOOST_INCLUDE_DIR="/absolute/path/containing/boost" -DARINC615A_TARGET_ONLY=ON -DARINC_615A_VXWORKS=ON -DARINC615A_BACKGROUND_THREAD=OFF -DARINC615A_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-vxworks --parallel 4
cmake --build build-vxworks --target arinc615a_audit
cmake --install build-vxworks --prefix build-vxworks/stage
```

Delete only `build-vxworks` and configure again whenever the SDK, BSP, CPU, ABI, or toolchain changes. Never reuse a cross-build cache from another machine or board.

## 5. Pass the build gate

Do not continue to Workbench until all of these are true:

- the build command exits successfully;
- the `arinc615a_audit` target exits successfully;
- `build-vxworks/stage/include/arinc_615a_tha.h` exists;
- all six archives exist in `build-vxworks/stage/lib`;
- `build-vxworks/stage/share/arinc615a/BUILD_INFO.txt` names the Wind River target compiler, not the host compiler;
- `DEPENDENCY_AUDIT.txt` does not report a forbidden desktop dependency.

Expected archives:

```text
libarinc_615a_test_tha_lib.a
libarinc_615a.a
libarinc_665.a
libtftp.a
libarinc_649.a
libhelper.a
```

Save `build.log`, `audit.log`, `CMakeCache.txt`, `compile_commands.json`, `BUILD_INFO.txt`, and `DEPENDENCY_AUDIT.txt`.

## 6. Create the final DKM in Workbench

Workbench names and menu positions vary by release, but the project settings must have the following meaning:

1. Create or open a **VxWorks Downloadable Kernel Module (DKM)** project for the actual board/VIP. Do not select RTP.
2. Enable both C and C++ support and use the SDK's C++ DKM linker flow.
3. Set the language standard to C++20.
4. Keep C++ exceptions and RTTI enabled.
5. Add `build-vxworks/stage/include` to the compiler include paths.
6. Add `examples/vxworks_entry.c` to the DKM project, or copy its two wrapper functions into the board application.
7. Add the six archives from `build-vxworks/stage/lib` to the DKM linker in this order:

```text
libarinc_615a_test_tha_lib.a
libarinc_615a.a
libarinc_665.a
libtftp.a
libarinc_649.a
libhelper.a
```

8. If the linker does a single left-to-right archive scan and reports unresolved ARINC symbols, use the Wind River linker setting for an archive group/rescan (or whole-archive where appropriate). Do not add desktop libraries to hide the error.
9. Build the final DKM with the selected BSP configuration.

Do not configure Workbench to compile every `.cpp` file in the repository. In particular, do not compile `app/arinc_615a_test_tha/arinc_615a_test_tha.cpp`; that is the desktop command-line program.

The DKM should export or make callable these wrapper symbols from `examples/vxworks_entry.c`:

```c
int arinc615aDkmRun(const char *configuration_file);
void arinc615aDkmStop(void);
```

## 7. Prepare the target configuration

Copy:

```text
build-vxworks/stage/share/arinc615a/arinc_615a_test_tha.json.template
```

to a new file named, for example, `arinc615a.json`. Change at least:

- `thwId`, `thwPosition`, and `target_id` to the real target identity;
- manufacturer, type, literal, serial, and part-number data;
- `local_tftp_address` and `local_find_address` if binding to a specific interface;
- TFTP and FIND ports if the platform/network requires different values;
- upload and download paths to absolute paths valid on the VxWorks target;
- check-value selections required by the integration.

Create the configured upload and download directories on the target before starting the service. They must be readable and writable by the DKM.

For Boost 1.86, use only `NotUsed`, `CRC8`, `CRC16`, `CRC32`, or `CRC64`. MD5/SHA choices require Boost 1.88 or newer.

Start with `log_level` set to `debug` for board bring-up, then reduce it after validation.

## 8. Connect and load the board

1. Connect the host and target on the approved isolated Ethernet network.
2. Boot the exact VxWorks image/VIP used for the DKM build.
3. Confirm host/target addressing and basic reachability using the site's normal Workbench target connection procedure.
4. Transfer `arinc615a.json` to the target filesystem.
5. Create the configured upload/download directories if they do not already exist.
6. Download/load the final DKM through Workbench.
7. Confirm the loader reports no unresolved symbols. If it does, stop here and resolve the first missing symbol before attempting to run.

Exact Workbench load buttons and target-shell module commands differ by VxWorks release and site configuration. Use the office project's established module loader; do not guess a loader command from another VxWorks version.

## 9. Start the THA in an application-owned task

`arinc615aDkmRun()` blocks until another task calls `arinc615aDkmStop()`. Therefore, spawn it as an application-owned VxWorks task using the BSP/project's normal task API or Workbench facility. Pass the absolute target path to the JSON file.

Conceptually, the worker task runs:

```c
arinc615aDkmRun("/target/path/arinc615a.json");
```

Do not call it directly from a shell context that must remain interactive unless that shell automatically creates a separate task.

The default DKM build has `ARINC615A_BACKGROUND_THREAD=OFF`. Calling `arinc615a_tha_start(1)` is therefore expected to return `ARINC615A_UNSUPPORTED`; the wrapper correctly uses blocking `arinc615a_tha_start(0)`.

## 10. Perform the first board test

Check these in order:

1. Initialization returns `ARINC615A_OK` (`0`).
2. The worker task remains alive and the service reports running.
3. The configured FIND UDP port binds successfully.
4. The configured TFTP port binds successfully.
5. A loader/host on the same isolated network can discover the THA.
6. An information operation returns the configured target identity.
7. Perform one small download and verify the received file and check value.
8. Perform one small upload and verify the received file and check value.
9. Confirm output is written only inside the configured directories.
10. Repeat stop/start once to prove lifecycle cleanup.

Do not begin with a large operational load. Use non-flight test data and small files until FIND, TFTP, paths, permissions, and checksums are proven.

## 11. Stop and unload safely

From a task other than the blocking worker task, call:

```c
arinc615aDkmStop();
```

Then:

1. wait for `arinc615aDkmStop()` to return;
2. wait for the worker task running `arinc615aDkmRun()` to return;
3. confirm no transfer is still active;
4. unload the DKM using the office project's normal module-unload procedure.

Never forcibly delete the worker task and never unload the DKM while its worker is still running.

## 12. If the first build or load fails

Fix only the first real compiler/linker/loader error; later errors are often cascading failures.

Collect these files before leaving the office PC:

```text
build-vxworks/build.log
build-vxworks/audit.log
build-vxworks/CMakeCache.txt
build-vxworks/compile_commands.json
build-vxworks/BUILD_INFO.txt
build-vxworks/DEPENDENCY_AUDIT.txt
```

Also record:

- the complete first failing command and error;
- Workbench, SDK, compiler, BSP, CPU, and kernel-image versions;
- the final DKM link command/map file;
- the first unresolved loader symbol, if any;
- console output from initialization and start;
- the edited JSON with sensitive/site-specific values redacted if necessary.

Common decisions:

| Failure | Next action |
| --- | --- |
| Host compiler shown in `BUILD_INFO.txt` | Use the Wind River DKM environment, remove `build-vxworks`, and reconfigure |
| `boost/version.hpp` missing | Point to the directory directly containing the `boost` folder |
| C++20 feature missing | Confirm the selected SDK compiler/runtime supports C++20 and Workbench did not downgrade the standard |
| Exception/RTTI symbols missing | Enable both and use the matching SDK C++ DKM linker/runtime |
| Socket/timer/filesystem symbols missing | Add the required components to the VxWorks image/VIP, then rebuild that image and the DKM consistently |
| ARINC symbols unresolved | Preserve archive order or enable an archive group/rescan |
| Module loads but initialization returns `-1` | Check JSON syntax, IDs, hash choice, paths, permissions, and port conflicts |
| Port already in use | Stop the conflicting service or select an approved unused port |
| Module will not unload safely | Stop from another task and wait for both stop and worker return |

## Definition of done

The port is complete only when the actual office configuration has passed all of the following:

- Wind River DKM cross-build and dependency audit;
- final DKM link with no unresolved symbols;
- load on the intended VxWorks image/BSP;
- valid JSON initialization;
- FIND discovery;
- one small information, download, and upload operation;
- clean stop, restart, second stop, and module unload;
- build logs and exact SDK/BSP details archived with the test result.

