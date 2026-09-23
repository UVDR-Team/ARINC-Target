# ARINC 615A: office build and debug

## Read this first

This is a self-contained **source** handoff, not a prebuilt VxWorks module.
All needed open-source libraries and generated headers are included. No Internet,
Git, Qt, external Helper, ARINC 649, or separate ARINC 645 library is needed.
Boost is required by the existing networking, hashing and utility implementation;
its headers are included and do not require a Boost runtime installation.

The host build and network tests are described in `VALIDATION.md`. **VxWorks 26.03
compilation, loading, and board debugging have not been verified here.** The
licensed office SDK, matching VSB/BSP, target image, CPU and ABI are not available
on the development Mac. It is not possible to guarantee a zero-error target build
without them. Workbench 4 alone is not the compiler or the target operating system.

## 1. Copy and open the right folder

Extract `ARINC615A_OFFICE_READY.zip` to a short local path, for example
`C:/ARINC/ARINC615A_OFFICE_READY`. Keep all its subfolders together.
Do not import the old full repository as a recursively compiled project.

In Workbench 4 create a **VxWorks Downloadable Kernel Module** managed project,
using this extracted folder as the project location (disable the default location).
Choose the office's VxWorks 26.03 platform and the **same VSB, BSP, CPU and ABI as
your running target image**. If the wizard requires an empty location, create the
project first and copy this folder's contents into it, then refresh the project.
Remove any wizard-generated example `main` source from the build.

The project wizard cannot safely be preselected on another computer: those SDK
and board settings belong to your office installation.

## 2. Set the build options once

Open project Properties / Build Properties (labels vary by installed Workbench).
Copy the four include directories and C++ compiler options from `BUILD_OPTIONS.txt`.
Select **only `src/` as build input**. Exclude `include/`, `tests/`, and the top-level
`workbench/` from compilation; they contain dependency headers or host tests.
`SOURCES.txt` is the exact production source list if the UI needs explicit inputs.
Enable normal managed DKM C++ initialization/constructor processing. Build Debug.

The SDK must provide the C++20 library features listed in `BUILD_OPTIONS.txt`,
especially `<format>` and `<filesystem>`. A missing standard-library header is an
SDK capability issue, not a missing Boost download. Do not lower `-std=c++20`.
Use the matching supported SDK/runtime configuration before continuing.

## 3. Check the target image

The target needs C++ runtime support, exceptions/RTTI, POSIX pthreads and
semaphores, time services, IPv4 UDP networking and `select`. File operations also
need a mounted filesystem. Your platform engineer must enable missing facilities
in the VSB/VIP; this source folder cannot change a running target image.

Connect Workbench's target connection to that image. Load the built `.out` module
with debug symbols using the normal Workbench download/debug workflow. A successful
compile is not sufficient: loading must also report no unresolved symbols.

## 4. First run: no JSON file or storage setup

Run `arinc615aSelfTest` as an entry function. Expect return `0` and
`ARINC self-test PASS`. This only checks codecs and SHA256, not board networking.

Set a breakpoint in `src/workbench/EntryPoints.cpp`, function `arinc615aDemo`.
Launch it as a dedicated kernel task with no arguments. Start with a 1 MiB task
stack, enable the task's floating-point context option when required by your BSP,
and inspect actual stack usage during testing. Do not call it from module startup
or a shell that must remain responsive: it blocks while the server runs.

Default demo: FIND UDP port `1001`, TFTP UDP port `59`, target ID `ARINC_1`, all
local interfaces. It supports FIND and Information; uploads/downloads are disabled
deliberately until storage paths are configured. Change defaults in
`src/workbench/DemoConfig.hpp` and rebuild if required. Configure the board IP and
loader IP on the same reachable network; allow UDP including negotiated transfer
ports through firewalls. The console prints the configured ports and target ID.

To stop, call `arinc615aStop` from another task/shell. Wait for the loader task to
exit before restarting, unloading or rebuilding the loaded module. Only one loader
runtime may run at a time. Never unload code while one of its tasks is running.

## 5. Enable upload and download

Edit `target-config.json`. Replace demo identifiers/part numbers with your actual
hardware data. Set directories to **real target filesystem paths**, not Windows
paths. Create the upload directory on writable target storage and populate the
download directory with the files you want the loader to retrieve.

Copy the JSON onto the target, for example `/ram0/ARINC/target-config.json` **only
if `/ram0` is actually mounted on your board**. Stop the demo, then launch
`arinc615aRun` in a dedicated task with that filename as its string argument.
Do not enable protocol-file logging until a valid writable log location is set.

The supplied transfer configuration enables Information, Upload, Media Defined
Download and Operator Defined Download. This is a protocol/file-transfer sample,
not board-specific firmware activation, flash programming, secure boot, or an
aviation-certified software-update installation procedure. Validate those separately.

## 6. Office acceptance checklist

- Clean Debug build succeeds with the correct 26.03 SDK and no unresolved symbols.
- Module loads; `arinc615aSelfTest` returns 0; debugger hits the demo breakpoint.
- Your real data loader discovers the correct hardware and reads Information.
- Upload a known test load; compare the resulting bytes/checksums.
- Test both download modes, rejected/malformed requests and timeout/abort behavior.
- Stop/restart and unload/reload cleanly; verify task stack and long-run memory usage.

Only after these checks should this be called VxWorks/board validated. Keep the
build log, target image configuration and data-loader results with the release.

Reference: Wind River's [SDK application guide](https://labs.windriver.com/downloads/wrsdk-vxworks7-docs/Application-Developer-Guide.html)
explains the matching SDK/VSB/VIP relationship and module/task workflow. It is not
evidence that this code has passed a VxWorks 26.03 build.
