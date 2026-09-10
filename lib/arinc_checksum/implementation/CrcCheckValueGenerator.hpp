// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class ArincChecksum::CrcCheckValueGenerator.
 **/

#ifndef ARINC_645_IMPLEMENTATION_CRCCHECKVALUEGENERATOR_HPP
#define ARINC_645_IMPLEMENTATION_CRCCHECKVALUEGENERATOR_HPP

#include <arinc_checksum/Arinc645.hpp>
#include <arinc_checksum/Arinc645Crc.hpp>
#include <arinc_checksum/CheckValueGenerator.hpp>

namespace ArincChecksum {

/**
 * @brief CRC Check Value Generator
 *
 * @tparam CrcGenerator
 *   CRC Generator Type
 **/
template< typename CrcGeneratorT, typename CrcValueT, CheckValueType CheckValueTypeV >
class CrcCheckValueGenerator final : public CheckValueGenerator
{
  public:
    //! CRC Generator
    using CrcGenerator = CrcGeneratorT;

    /**
     * @brief CRC Value Type
     *
     * We cannot relay on @p CrcGeneratorT::value_type as this type does not provide the exact width type.
     **/
    using CrcValueType = CrcValueT;

    //! Check Value Type
    static constexpr CheckValueType Type{ CheckValueTypeV };

    //! Constructs CRC Check Value Generator
    CrcCheckValueGenerator() = default;

    //! @copydoc CheckValueGenerator::reset
    void reset() override;

    //! @copydoc CheckValueGenerator::process
    void process( ArincSupport::ConstRawDataSpan data ) override;

    //! @copydoc CheckValueGenerator::checkValue()
    [[nodiscard]] CheckValue checkValue() override;

  private:
    //! CRC Generator
    CrcGenerator crcGenerator;
};

//! CRC 8 Check Value Generator
using Crc8CheckValueGenerator = CrcCheckValueGenerator< Arinc645Crc8, uint8_t, CheckValueType::Crc8 >;
//! CRC 16 Check Value Generator
using Crc16CheckValueGenerator = CrcCheckValueGenerator< Arinc645Crc16, uint16_t, CheckValueType::Crc16 >;
//! CRC 32 Check Value Generator
using Crc32CheckValueGenerator = CrcCheckValueGenerator< Arinc645Crc32, uint32_t, CheckValueType::Crc32 >;
//! CRC 64 Check Value Generator
using Crc64CheckValueGenerator = CrcCheckValueGenerator< Arinc645Crc64, uint64_t, CheckValueType::Crc64 >;

}

#include <arinc_checksum/implementation/CrcCheckValueGenerator.ipp>

#endif
