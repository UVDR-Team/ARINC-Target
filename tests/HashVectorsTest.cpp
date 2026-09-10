// SPDX-License-Identifier: MPL-2.0
#include <arinc_checksum/CheckValueGenerator.hpp>
#include <arinc_checksum/CheckValue.hpp>
#include <arinc_support/RawData.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(standard_hash_vectors)
{
  using ArincChecksum::CheckValueType;
  const auto input = ArincSupport::RawData_asRaw("abc");
  // Published standard algorithm test vectors, independent of serialization.
  const std::pair<CheckValueType, const char *> vectors[] = {
    {CheckValueType::Md5, "900150983cd24fb0d6963f7d28e17f72"},
    {CheckValueType::Sha1, "a9993e364706816aba3e25717850c26c9cd0d89d"},
    {CheckValueType::Sha256, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"},
    {CheckValueType::Sha512, "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f"}
  };
  for (const auto &[type, expected] : vectors)
  {
    const auto actual = ArincChecksum::CheckValueGenerator::checkValue(type, input);
    BOOST_REQUIRE(actual.has_value());
    BOOST_CHECK(*actual == ArincChecksum::CheckValue(type, expected));
  }
}
