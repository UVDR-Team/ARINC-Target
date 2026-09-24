// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of ARINC 645 CRCs.
 **/

#ifndef ARINC_645_ARINC645CRC_HPP
#define ARINC_645_ARINC645CRC_HPP

#include <arinc_checksum/Arinc645.hpp>

#include <boost/crc.hpp>

#include <cstdint>

namespace ArincChecksum {

/**
 * @name ARINC 645 CRC-8
 * @{
 **/

//! CRC 8bit Polynom
constexpr uint8_t Crc8Polynom{ 0x01U };
//! CRC 8bit initialisation value
constexpr uint8_t Crc8Init{ 0x00U };
//! CRC 8bit final XOR value
constexpr uint8_t Crc8FinalXor{ 0x00U };
//! CRC 8bit reflect in
constexpr bool Crc8ReflectIn{ false };
//! CRC 8bit reflect out
constexpr bool Crc8ReflectOut{ false };

//! ARINC 645 CRC 8 Definition
using Arinc645Crc8 =
  boost::crc_optimal<
    8U,
    Crc8Polynom,
    Crc8Init,
    Crc8FinalXor,
    Crc8ReflectIn,
    Crc8ReflectOut >;

/** @} **/

/**
 * @name ARINC 645 CRC-16
 * @{
 **/

//! CRC 16bit Polynom
constexpr uint16_t Crc16Polynom{ 0x1021U };
//! CRC 16bit initialisation value
constexpr uint16_t Crc16Init{ 0xFFFFU };
//! CRC 16bit final XOR value
constexpr uint16_t Crc16FinalXor{ 0x0000U };
//! CRC 16bit reflect in
constexpr bool Crc16ReflectIn{ false };
//! CRC 16bit reflect out
constexpr bool Crc16ReflectOut{ false };

//! ARINC 645 CRC 16 Definition
using Arinc645Crc16 =
  boost::crc_optimal<
    16U,
    Crc16Polynom,
    Crc16Init,
    Crc16FinalXor,
    Crc16ReflectIn,
    Crc16ReflectOut >;

/** @} **/

/**
 * @name ARINC 645 CRC-32
 * @{
 **/

//! CRC 32bit Polynom
constexpr uint32_t Crc32Polynom{ 0x04C1'1DB7U };
//! CRC 32bit initialisation value
constexpr uint32_t Crc32Init{ 0xFFFF'FFFFU };
//! CRC 32bit final XOR value
constexpr uint32_t Crc32FinalXor{ 0xFFFF'FFFFU };
//! CRC 32bit reflect in
constexpr bool Crc32ReflectIn{ false };
//! CRC 32bit reflect out
constexpr bool Crc32ReflectOut{ false };

//! ARINC 645 CRC 32 Definition
using Arinc645Crc32 =
  boost::crc_optimal<
    32U,
    Crc32Polynom,
    Crc32Init,
    Crc32FinalXor,
    Crc32ReflectIn,
    Crc32ReflectOut >;

/** @} **/

/**
 * @name ARINC 645 CRC-64
 *
 * @todo According to ARINC 645-1 Reflect In/Out should be set to true - but then the tests fail!
 *
 * @{
 **/

//! CRC 64bit Polynom
constexpr uint64_t Crc64Polynom{ 0x42F0'E1EB'A9EA'3693U };
//! CRC 64bit initialisation value
constexpr uint64_t Crc64Init{ 0xFFFF'FFFF'FFFF'FFFFU };
//! CRC 64bit final XOR value
constexpr uint64_t Crc64FinalXor{ 0xFFFF'FFFF'FFFF'FFFFU };
//! CRC 64bit reflect in
constexpr bool Crc64ReflectIn{ false };
//! CRC 64bit reflect out
constexpr bool Crc64ReflectOut{ false };

//! ARINC 645 CRC 64 Definition
using Arinc645Crc64 =
  boost::crc_optimal<
    64U,
    Crc64Polynom,
    Crc64Init,
    Crc64FinalXor,
    Crc64ReflectIn,
    Crc64ReflectOut >;

/** @} **/

}

#endif
