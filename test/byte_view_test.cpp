/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <qtils/byte_view.hpp>

#include <gtest/gtest.h>

#include <qtils/test/outcome.hpp>

#include "qtils/byte_vec.hpp"

using qtils::ByteView;
using Span = std::span<uint8_t>;
using namespace std::string_literals;

/**
 * @test Default-constructed ByteView.
 * @given default-constructed ByteView
 * @when checking its size and content
 * @then the view is empty and has size 0
 */
TEST(ByteView, Constructor_default) {
  ByteView v;

  EXPECT_EQ(v.toHex(), "<empty>"s);
  EXPECT_EQ(v.size(), 0);
}

/**
 * @test ByteView constructed from span.
 * @given a span of bytes
 * @when constructing a ByteView from it
 * @then the ByteView reflects the content and size of the span
 */
TEST(ByteView, Constructor_from_span) {
  uint8_t c_arr[] = {1, 2, 3, '1', '2', '3'};
  Span span(std::begin(c_arr), std::end(c_arr));

  ByteView view_span(span);

  EXPECT_EQ(view_span.toHex(), "010203313233");
  EXPECT_EQ(view_span.size(), std::size(c_arr));
}

/**
 * @test ByteView constructed from vector.
 * @given a vector of bytes
 * @when constructing a ByteView from it
 * @then the ByteView reflects the content and size of the vector
 */
TEST(ByteView, Constructor_from_vector) {
  std::vector<uint8_t> vec = {1, 2, 3, '1', '2', '3'};

  ByteView view_vec(vec);

  EXPECT_EQ(view_vec.toHex(), "010203313233");
  EXPECT_EQ(view_vec.size(), vec.size());
}

/**
 * @test ByteView constructed from array.
 * @given a std::array of bytes
 * @when constructing a ByteView from it
 * @then the ByteView reflects the content and size of the array
 */
TEST(ByteView, Constructor_from_array) {
  std::array<uint8_t, 6> arr = {1, 2, 3, '1', '2', '3'};

  ByteView view_arr(arr);

  EXPECT_EQ(view_arr.toHex(), "010203313233");
  EXPECT_EQ(view_arr.size(), arr.size());
}

/**
 * @test ByteView constructed from another ByteView.
 * @given an existing ByteView
 * @when constructing another ByteView from it
 * @then the new ByteView has the same content and size
 */
TEST(ByteView, Constructor_from_ByteView) {
  std::array<uint8_t, 6> arr = {1, 2, 3, '1', '2', '3'};
  ByteView view_arr(arr);

  ByteView view_view(view_arr);

  EXPECT_EQ(view_view.toHex(), "010203313233");
  EXPECT_EQ(view_view.size(), arr.size());
}

/**
 * @test ByteView formatting with fmtlib.
 * @given A ByteView initialized with the bytes
 * @when Formatted using fmt::format with some format specifiers
 * @then The formatted strings match the expected representations
 */
TEST(ByteView, Format) {
  uint8_t arr[] = {0x01, 0x02, 0x03, 0x0a, 0x0b, 0x0c};
  ByteView view(arr);

  ASSERT_EQ(fmt::format("{:0x}", view),  "0x0102…0b0c"s);
}
