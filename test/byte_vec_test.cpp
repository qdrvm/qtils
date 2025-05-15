/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <qtils/byte_vec.hpp>

#include <gtest/gtest.h>

using qtils::byte_t;
using qtils::ByteVec;
using namespace std::string_literals;

/// Case-insensitive string comparison helper
bool iequals(std::string_view a, std::string_view b) {
  return a.size() == b.size()
      and std::ranges::equal(
          a,
          b,
          {},
          [](char ch) { return std::tolower(static_cast<unsigned char>(ch)); },
          [](char ch) { return std::tolower(static_cast<unsigned char>(ch)); });
}

/**
 * @test Writing data into buffer via various put-methods.
 * @given empty buffer
 * @when data is sequentially appended using string, integers, and another
 * buffer
 * @then resulting buffer size and content match expectations
 */
TEST(ByteVecTest, Put) {
  ByteVec b;
  ASSERT_EQ(b.size(), 0);

  auto hex = b.toHex();
  ASSERT_EQ(hex, ""s);

  auto s = "hello"s;
  b.put(s);
  ASSERT_EQ(b.size(), 5);

  b.putUint8(1);
  ASSERT_EQ(b.size(), 6);

  b.putUint32(1);
  ASSERT_EQ(b.size(), 10);

  b.putUint64(1);
  ASSERT_EQ(b.size(), 18);

  std::vector<uint8_t> e{1, 2, 3, 4, 5};
  b.put(e);
  ASSERT_EQ(b.size(), 23);

  int i = 0;
  for (const auto &byte : b) {
    i++;
    (void)byte;
  }
  ASSERT_EQ(i, b.size());

  ASSERT_TRUE(
      iequals(b.toHex(), "68656c6c6f010000000100000000000000010102030405"));
}

/**
 * @test ByteVec appending.
 * @given a buffer with initial content and another buffer
 * @when appending the second buffer using put
 * @then the first buffer contains the concatenated result
 */
TEST(ByteVecTest, PutByteVec) {
  ByteVec current_buffer = {1, 2, 3};
  ByteVec another_buffer = {4, 5, 6};
  auto &buffer = current_buffer.put(another_buffer);
  ASSERT_EQ(&buffer, &current_buffer);
  ByteVec result = {1, 2, 3, 4, 5, 6};
  ASSERT_EQ(buffer, result);
}

/**
 * @test ByteVec initialization.
 * @given various buffer initialization methods
 * @when constructing buffers via initializer list, copy, and array
 * @then the created buffers are equivalent and contain the same data
 */
TEST(ByteVecTest, Init) {
  std::array<uint8_t, 5> data = {1, 2, 3, 4, 5};

  ByteVec b{1, 2, 3, 4, 5};
  ASSERT_EQ(b.size(), 5);
  ASSERT_TRUE(iequals(b.toHex(), "0102030405"s));

  ByteVec a(b);
  ASSERT_EQ(a, b);
  ASSERT_EQ(a.size(), b.size());

  ByteVec c(data);
  ASSERT_EQ(a, c);
  ASSERT_EQ(a.size(), c.size());
}

/**
 * @test ByteVec formatting with fmtlib.
 * @given A ByteVec initialized with the bytes
 * @when Formatted using fmt::format with some format specifiers
 * @then The formatted strings match the expected representations
 */
TEST(ByteVecTest, Format) {
  ByteVec vec{0x01, 0x02, 0x03, 0x0a, 0x0b, 0x0c};

  ASSERT_EQ(fmt::format("{:0x}", vec),  "0x0102…0b0c"s);
}
