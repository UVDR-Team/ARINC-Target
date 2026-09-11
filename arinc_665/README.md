# ARINC 665 Tool Suite
![ProjectLogo.svg](ProjectLogo.svg)

Provides library and applications for generating, reading and handling ARINC 665 Media Sets.

This tool suite supports the generation and interpretation of media sets according to ARINC 665-2, ARINC 665-3, ARINC 665-4, and ARINC 665-5.

A library is provided for:
- generating and parsing ARINC 665 files
- media set model, and
- utilities for generation and management of media sets.

An XML format and corresponding tools are provided to ease generation of media sets.

For building graphical applications a Qt based library is provided which consist of models, dialogues, and wizards to work with ARINC 665 media sets.

## Dependencies
First level dependencies:
- [Helper Library](https://git.thomas-vogt.de/thomas-vogt/helper)
- [Qt Icon Resources](https://git.thomas-vogt.de/thomas-vogt/qt_icon_resources)
- [ARINC 649 Tool Suite](https://git.thomas-vogt.de/thomas-vogt/arinc-649)
- [Commands Library](https://git.thomas-vogt.de/thomas-vogt/commands)
- [Boost Libraries](https://www.boost.org/)
- libxml++
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

## Changes
This section only contains changes within the standards, which are respected within this library.

### ARINC 665-1
- Update file versions (Separate between media files, load files)
- FILES.LUM: Add filename and file length field
- Add optional batch files (.LUB)
- Add optional list of batch files (BATCHES.LUM)

### ARINC 665-2
- Update File Versions (Separate between media files, load files, and batch files)
- Load header file (.LUH): add `spare field` after file length and file format version to align following fields to 32 bit
- Batch file (.LUB): add `spare field` after file length and file format version to align following fields to 32 bit
- LOADS.LUM file: add `spare field` after file length and file format version to align following fields to 32 bit
- FILES.LUM file: add `spare field` after file length and file format version to align following fields to 32 bit
- BATCHES.LUM file: add `spare field` after file length and file format version to align following fields to 32 bit

### ARINC 665-3
- Update File Versions (Separate between media files, load files, and batch files)
- Load header file (.LUH)
  - Change *Spare* field to *Part Flags* field
  - Add *Load Check Value*
  - Add *Load Type Section* (ID + description)
  - Add *Target Hardware IDs with Positions* section
  - Add Data File *length in bytes* field
  - Add *Data and Support File Check Value* field
- FILES.LUM add *File Check Value*

### ARINC 665-4
Clarification but no changes of data formats.

### ARINC 665-5
Move Checksum and Check Value definition to ARINC 645 and later ARINC 649.
No changes of data format.

## References
- [ARINC Report 665-5](https://aviation-ia.sae-itc.com/standards/arinc665-5-665-5-loadable-software-standards):
  Loadable Software Standards;
  2019-08-13
- [ARINC Report 649](https://aviation-ia.sae-itc.com/standards/arinc649-arinc-649-common-terminology-functions-software-distribution-loading):
  Common Terminology and Functions for Software Distribution and Loading;
  2025-12-02
