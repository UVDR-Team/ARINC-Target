# ARINC 649 Tool Suite Documentation {#mainpage}

This library provides an implementation, according to _ARINC 649_.
This standard is used within the standards _ARINC 665_ and _ARINC 615A_.
The main functionality is to provide common interfaces for:
- Checksums (CRC-8, CRC-16, CRC-32, and CRC-64),
- Check Value (all Checksums as above and MD5, SHA1, SHA-256, and SHA2-512), and
- File naming.

For CRC calculation, this library depends on the Boost.Crc library.
For the MD5, SHA, SHA-256, and SHA-512 calculation, this library depends on the Boost.Hash2 library.

## Libraries

### arinc_649 (C++ library)
Core C++ implementation of ARINC 649 common terminology and functions:
- Check value types and generators (CRC family, MD5, SHA1, SHA-256, SHA-512)
- Check value representation and verification utilities
- Standard-compliant file naming helpers
- Lightweight exceptions and version information
This library has no GUI dependency and is intended for use in headless tools, services, and other standards-based components.

### arinc_649_qt (C++ library)
Qt integration layer for ARINC 649:
- Qt-friendly wrappers around ARINC 649 types and operations
- Convenience classes to integrate check value generation and file naming into Qt applications
- Ready-to-use building blocks for GUI tools (signals/slots and model-friendly types)
Use this library when building Qt-based applications that consume ARINC 649 functionality.

This tool suite contains the components:
- ARINC 649
  - ARINC 649 Standard Namespace @ref Arinc649, providing the core ARINC 649 functionality.
  - ARINC 649 Standard Qt Framework Namespace @ref Arinc649Qt, providing Qt integration utilities.
  - @subpage arinc_649_applications,
- %Helper Library
  - %Helper Library Namespace @ref Helper,
  - %Helper Qt Framework Namespace @ref HelperQt,
  - @subpage helper_applications.
