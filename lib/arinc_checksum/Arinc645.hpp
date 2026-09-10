// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc645.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc645.
 **/

#ifndef ARINC_645_ARINC645_HPP
#define ARINC_645_ARINC645_HPP

#include <arinc_checksum/arinc_645_export.h>

#include <arinc_support/Support.hpp>

#include <cstdint>
#include <memory>

/**
 * @brief ARINC 645 Specification.
 *
 * This library provides an implementation, according to _ARINC 645-1_.
 * This standard is used within the standards _ARINC 665_ and _ARINC 615A_.
 * The main functionality is to provide common interfaces for:
 * - Checksum calculation algorithms (CRC-8, CRC-16, CRC-32, and CRC-64) see @ref Arinc645Crc.hpp
 *   (ARINC 645-1 Section 4.3)
 * - Check Value (ARINC 645-1 Section 4ff) representation and calculation @ref CheckValue and @ref CheckValueGenerator
 *   (all Checksums as above and MD5, SHA1, SHA-256, and SHA2-512), and
 * - File Naming @ref Filename.hpp (ARINC 645-1 Section 7.2)
 *
 * For CRC calculation, this library depends on the Boost.Crc library.
 * For the MD5, SHA, SHA-256, and SHA-512 calculation, this library depends on the Boost.Hash2 library.
 *
 * @par Referenced Documents
 * - ARINC Report 645: Common Terminology and Functions for Software Distribution and Loading; 2018-07-11
 * - ARINC Report 645-1: Common Terminology and Functions for Software Distribution and Loading; 2021-08-11
 **/
namespace ArincChecksum {

//! Check Value Type Enumeration
enum class CheckValueType : uint16_t
{
  //! Not to be used
  NotUsed = 0U,
  //! 8-bit CRC (deprecated)
  //! @deprecated Use Crc32 or Crc64 instead.
  Crc8 = 1U,
  //! 16-bit CRC (deprecated)
  //! @deprecated Use Crc32 or Crc64 instead.
  Crc16 = 2U,
  //! 32-bit CRC
  Crc32 = 3U,
  //! Message-Digest Algorithm 5 (MD5)
  //! @deprecated Use Sha1, Sha256, or Sha512 instead.
  Md5 = 4U,
  //! Secure Hash Algorithm SHA1
  Sha1 = 5U,
  //! Secure Hash Algorithm SHA-256
  Sha256 = 6U,
  //! Secure Hash Algorithm SHA-512
  Sha512 = 7U,
  //! 64-bit CRC
  Crc64 = 8U
};

class CheckValue;
class CheckValueTypeDescription;
class CheckValueGenerator;

//! Check Value Generator Instance.
using CheckValueGeneratorPtr = std::shared_ptr< CheckValueGenerator >;

}

#endif
