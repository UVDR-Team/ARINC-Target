# ARINC 615A target-side DKM integration

The VxWorks profile builds `arinc_615a_tha_target` as a static C++ library.
It contains the target-side FIND, information, upload, media-defined download,
and operator-defined download behaviour.  It does not build the GUI, host-side
operations, command-line tools, unit-test objects, or documentation.

`spdlog` is replaced by the local header-only compatibility layer in
`lib/spdlog/spdlog.h`.  Diagnostic logging is compiled out, so the DKM has no
logger runtime dependency and no logger-created threads, files, or heap use.

## Required protocol libraries

TFTP, helper, ARINC 645, and ARINC 665 are not removable dependencies: the
target protocol uses their interfaces to transfer and parse ARINC files.  They
must be supplied as VxWorks libraries compiled for the exact same CPU, ABI,
C++23 setting, and C++ runtime as the DKM.

Copy `VxWorksDKMDependencies.cmake.example` to an integration-owned location,
replace every placeholder path, then configure with:

```text
-DARINC615A_DKM_PROFILE=ON
-DARINC615A_DEPENDENCY_FILE=/absolute/path/to/your/VxWorksDKMDependencies.cmake
```

The source tree never calls Git or CMake `FetchContent`; the dependency file is
the only mechanism for supplying those required prebuilt libraries.

## C wrapper entry point

The original executable logic is now the C++ function
`Arinc615aTha::run(int argc, char *argv[])`, declared in
`app/arinc_615a_unit_test/arinc_615a_test_tha/Arinc615aTha.hpp`.  The optional
host executable consists only of `main.cpp` forwarding to that function.

The C wrapper should invoke `Arinc615aTha::run` from the DKM's dedicated
worker task, not from the DKM initialization routine: the call deliberately
runs the ASIO event loop until a termination request.  Its argument zero is
used to locate the existing JSON configuration file, so retain the original
configuration filename convention or adapt the wrapper to pass the desired
path.
