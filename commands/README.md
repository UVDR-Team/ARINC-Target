# Commands C++ Library
![ProjectLogo.svg](ProjectLogo.svg)

This project provides a library for command registration and execution.
It is implemented in C++.
The *Commands Library* uses boost::program_options for command line parsing.

## Dependencies
- [Helper Library](https://git.thomas-vogt.de/thomas-vogt/helper)
- [Boost Libraries](https://www.boost.org/)
  - Boost.Asio
  - Boost.Endian
  - Boost.Exception
  - Boost.MultiIndex
  - Boost.ProgramOptions
  - Boost.Serialization
  - Boost.Test
- [spdlog](https://github.com/gabime/spdlog)

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
