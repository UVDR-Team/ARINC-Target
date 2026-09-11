# ARINC 649 Tool Suite

This project provides an implementation, according to _ARINC 649_.
This standard is used within the standards _ARINC 665_ and _ARINC 615A_.
The main functionality is to provide common interfaces for:
- Checksums (CRC-8, CRC-16, CRC-32, and CRC-64)
- Check Value (all Checksums as above and MD5, SHA1, SHA-256, and SHA2-512), and
- File naming.

For CRC calculation, this library depends on the Boost.Crc library.
For the MD5, SHA, SHA-256, and SHA-512 calculation, this library depends on the
Boost.Hash2 library.

## Dependencies
First level dependencies:
- [Helper Library](https://git.thomas-vogt.de/thomas-vogt/helper)
- [Boost Libraries](https://www.boost.org/)
  - Boost.Asio
  - Boost.Chrono
  - Boost.Crc
  - Boost.Exception
  - Boost.Hash2
  - Boost.MultiIndex
  - Boost.ProgramOptions
  - Boost.PropertyTree
  - Boost.Serialization
  - Boost.Test
- [spdlog](https://github.com/gabime/spdlog)
- Optionally [Qt 6](https://www.qt.io/)

## Building
The library uses [CMake](https://cmake.org/) to handle build configuration.
CMake Presets are provided to generate builds compiling with:
- GNU GCC,
- Clang, and
- MSVC.

For each compiler and environment, the following variants can be built:
- Static debug,
- Static release,
- Shared debug, and
- Shared release.

Test Environments are:
- Linux,
- Windows MinGW, and
- Windows MSVC.

**Note:**
For managing dependencies, i.e. Windows MSVC, a VCPKG configuration is provided.

## License
This project is licensed under the terms of the [*Mozilla Public License Version 2.0* (MPL)](LICENSE).

## References
- [ARINC Report 649](https://aviation-ia.sae-itc.com/standards/arinc649-arinc-649-common-terminology-functions-software-distribution-loading):
  Common Terminology and Functions for Software Distribution and Loading;
  2025-12-02
