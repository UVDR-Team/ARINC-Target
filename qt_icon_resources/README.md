# Qt Icon Resources
![ProjectLogo.svg](ProjectLogo.svg)

This project provides a library with commonly used icons.
The icons are taken from the Font-Awesome project.
The Version 7.3.1 is used.

For updates the `svgs-full` folder is used.

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
- [Qt 6](https://www.qt.io/)

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
The font resources (SVGs) are licensed under the *CC BY 4.0 License*.

## References
- [Font Awesome on GitHub](https://github.com/FortAwesome/Font-Awesome)
