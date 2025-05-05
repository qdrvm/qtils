/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <qtils/byte_arr.hpp>

using qtils::byte_t;
using qtils::ByteArr;

/// Case-insensitive string comparison helper
bool iequals(std::string_view a, std::string_view b) {
  return a.size() == b.size()
      && std::ranges::equal(
          a,
          b,
          {},
          [](char ch) { return std::tolower(static_cast<unsigned char>(ch)); },
          [](char ch) { return std::tolower(static_cast<unsigned char>(ch)); });
}

/**
 * @test ByteArr creation from valid hex-encoded string.
 * @given valid hex-encoded string
 * @when creating a ByteArr from it
 * @then the ByteArr is successfully created and contains the correct data
 */
TEST(ByteArrTest, CreateFromValidHex) {
  std::string hex32 = "00ff";
  std::array<byte_t, 2> expected{0, 255};

  auto result = ByteArr<2>::fromHex(hex32);
  ASSERT_NO_THROW({
    auto blob = result.value();
    EXPECT_EQ(blob, expected);
  }) << "fromHex returned an error instead of value";
}

/**
 * @test ByteArr creation fails on non-hex string.
 * @given string that is not hex-encoded
 * @when creating a ByteArr from it
 * @then an error is returned
 */
TEST(ByteArrTest, CreateFromNonHex) {
  std::string not_hex = "nothex";

  auto result = ByteArr<2>::fromHex(not_hex);
  ASSERT_NO_THROW({ result.error(); })
      << "fromHex returned a value instead of error";
}

/**
 * @test ByteArr creation fails on odd-length hex string.
 * @given hex-encoded string of odd length
 * @when creating a ByteArr from it
 * @then an error is returned
 */
TEST(ByteArrTest, CreateFromOddLengthHex) {
  std::string odd_hex = "0a1";

  auto result = ByteArr<2>::fromHex(odd_hex);
  ASSERT_NO_THROW({ result.error(); })
      << "fromHex returned a value instead of error";
}

/**
 * @test ByteArr creation fails on incorrect hex length.
 * @given hex-encoded string of incorrect length
 * @when creating a ByteArr with a fixed size from it
 * @then an error is returned
 */
TEST(ByteArrTest, CreateFromWrongLendthHex) {
  std::string odd_hex = "00ff00";

  auto result = ByteArr<2>::fromHex(odd_hex);
  ASSERT_NO_THROW({ result.error(); })
      << "fromHex returned a value instead of error";
}

/**
 * @test ByteArr creation from valid string.
 * @given string of valid length
 * @when creating a ByteArr from it using fromString
 * @then the ByteArr is successfully created and contains the expected data
 */
TEST(ByteArrTest, CreateFromValidString) {
  std::array<byte_t, 5> expected{'a', 's', 'd', 'f', 'g'};
  std::string valid_str{expected.begin(), expected.end()};

  auto result = ByteArr<5>::fromString(valid_str);
  ASSERT_NO_THROW({
    auto blob = result.value();
    EXPECT_EQ(blob, expected);
  }) << "fromString returned an error instead of value";
}

/**
 * @test ByteArr creation fails on string of incorrect length.
 * @given string of incorrect length
 * @when creating a ByteArr from it using fromString
 * @then an error is returned
 */
TEST(ByteArrTest, CreateFromInvalidString) {
  std::string valid_str{"0"};

  auto result = ByteArr<5>::fromString(valid_str);
  ASSERT_NO_THROW({ result.error(); })
      << "fromString returned a value instead of error";
}

/**
 * @test Conversion of ByteArr content to hex string.
 * @given string content
 * @when creating a ByteArr and converting it to hex
 * @then the result matches the expected hex representation
 */
TEST(ByteArrTest, ToHexTest) {
  std::string str = "hello";
  std::string expected_hex = "68656c6c6f";

  auto blob_res = ByteArr<5>::fromString(str);
  ASSERT_NO_THROW({
    ByteArr<5> value = blob_res.value();
    ASSERT_TRUE(iequals(value.toHex(), expected_hex));
  });
}

/**
 * @test Conversion of ByteArr content to string.
 * @given binary content
 * @when creating a ByteArr and calling toString
 * @then the result matches the original string
 */
TEST(ByteArrTest, ToStringTest) {
  std::array<byte_t, 5> expected{'a', 's', 'd', 'f', 'g'};

  ByteArr<5> blob;
  std::ranges::copy(expected, blob.begin());

  ASSERT_TRUE(
      iequals(blob.toString(), std::string(expected.begin(), expected.end())));
}
