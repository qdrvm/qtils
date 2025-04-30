/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <qtils/buffer.hpp>

#include <gtest/gtest.h>

using qtils::Buffer;
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
TEST(BufferTest, Put) {
  Buffer b;
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
 * @test Buffer appending.
 * @given a buffer with initial content and another buffer
 * @when appending the second buffer using put
 * @then the first buffer contains the concatenated result
 */
TEST(BufferTest, PutBuffer) {
  Buffer current_buffer = {1, 2, 3};
  Buffer another_buffer = {4, 5, 6};
  auto &buffer = current_buffer.put(another_buffer);
  ASSERT_EQ(&buffer, &current_buffer);
  Buffer result = {1, 2, 3, 4, 5, 6};
  ASSERT_EQ(buffer, result);
}

/**
 * @test Buffer initialization.
 * @given various buffer initialization methods
 * @when constructing buffers via initializer list, copy, and array
 * @then the created buffers are equivalent and contain the same data
 */
TEST(BufferTest, Init) {
  std::array<uint8_t, 5> data = {1, 2, 3, 4, 5};

  Buffer b{1, 2, 3, 4, 5};
  ASSERT_EQ(b.size(), 5);
  ASSERT_TRUE(iequals(b.toHex(), "0102030405"s));

  Buffer a(b);
  ASSERT_EQ(a, b);
  ASSERT_EQ(a.size(), b.size());

  Buffer c(data);
  ASSERT_EQ(a, c);
  ASSERT_EQ(a.size(), c.size());
}
