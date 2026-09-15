// SPDX-License-Identifier: MPL-2.0
#include <helper/RawData.hpp>
#include <helper/EndianDescription.hpp>
#include <helper/Version.hpp>
#include <arinc_649/CheckValueGenerator.hpp>
#include <arinc_649/CheckValue.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>
BOOST_AUTO_TEST_SUITE(TargetPortability)
BOOST_AUTO_TEST_CASE(metadata_and_endian_alias)
{
  BOOST_CHECK_EQUAL(Helper::Version::Key, "helper");
  BOOST_CHECK(Helper::EndianDescription::instance().enumeration("native") == std::endian::native);
}
BOOST_AUTO_TEST_CASE(unaligned_network_integer)
{
  alignas(8) std::array<std::byte, 10> bytes{};
  auto buffer = Helper::RawDataSpan(bytes).subspan(1);
  Helper::RawData_setInt<std::uint32_t>(buffer, 0x12345678U, boost::endian::order::big);
  const auto [remaining, value] = Helper::RawData_getInt<std::uint32_t>(buffer, boost::endian::order::big);
  BOOST_CHECK_EQUAL(value, 0x12345678U);
  BOOST_CHECK(bytes[1] == std::byte{0x12});
  BOOST_CHECK(bytes[4] == std::byte{0x78});
}
BOOST_AUTO_TEST_CASE(hash_availability)
{
  using Arinc649::CheckValueType;
  for (const auto type : {CheckValueType::NotUsed, CheckValueType::Crc8, CheckValueType::Crc16,
       CheckValueType::Crc32, CheckValueType::Crc64})
    BOOST_CHECK(Arinc649::CheckValueGenerator::create(type));
  for (const auto type : {CheckValueType::Md5, CheckValueType::Sha1, CheckValueType::Sha256, CheckValueType::Sha512})
  {
#if BOOST_VERSION >= 108800
    BOOST_CHECK(Arinc649::CheckValueGenerator::create(type));
#else
    BOOST_CHECK(!Arinc649::CheckValueGenerator::create(type));
#endif
  }
}
BOOST_AUTO_TEST_SUITE_END()
