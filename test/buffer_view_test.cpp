/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <qtils/byte_view.hpp>

#include <gtest/gtest.h>

#include <qtils/test/outcome.hpp>

using qtils::ByteView;
using Span = std::span<uint8_t>;

/**
 * @test Default-constructed BufferView.
 * @given default-constructed BufferView
 * @when checking its size and content
 * @then the view is empty and has size 0
 */
TEST(BufferView, Constructor_default) {
  ByteView v;

  EXPECT_EQ(v.toHex(), "");
  EXPECT_EQ(v.size(), 0);
}

/**
 * @test BufferView constructed from span.
 * @given a span of bytes
 * @when constructing a BufferView from it
 * @then the BufferView reflects the content and size of the span
 */
TEST(BufferView, Constructor_from_span) {
  uint8_t c_arr[] = {1, 2, 3, '1', '2', '3'};
  Span span(std::begin(c_arr), std::end(c_arr));

  ByteView view_span(span);

  EXPECT_EQ(view_span.toHex(), "010203313233");
  EXPECT_EQ(view_span.size(), std::size(c_arr));
}

/**
 * @test BufferView constructed from vector.
 * @given a vector of bytes
 * @when constructing a BufferView from it
 * @then the BufferView reflects the content and size of the vector
 */
TEST(BufferView, Constructor_from_vector) {
  std::vector<uint8_t> vec = {1, 2, 3, '1', '2', '3'};

  ByteView view_vec(vec);

  EXPECT_EQ(view_vec.toHex(), "010203313233");
  EXPECT_EQ(view_vec.size(), vec.size());
}

/**
 * @test BufferView constructed from array.
 * @given a std::array of bytes
 * @when constructing a BufferView from it
 * @then the BufferView reflects the content and size of the array
 */
TEST(BufferView, Constructor_from_array) {
  std::array<uint8_t, 6> arr = {1, 2, 3, '1', '2', '3'};

  ByteView view_arr(arr);

  EXPECT_EQ(view_arr.toHex(), "010203313233");
  EXPECT_EQ(view_arr.size(), arr.size());
}

/**
 * @test BufferView constructed from another BufferView.
 * @given an existing BufferView
 * @when constructing another BufferView from it
 * @then the new BufferView has the same content and size
 */
TEST(BufferView, Constructor_from_BufferView) {
  std::array<uint8_t, 6> arr = {1, 2, 3, '1', '2', '3'};
  ByteView view_arr(arr);

  ByteView view_view(view_arr);

  EXPECT_EQ(view_view.toHex(), "010203313233");
  EXPECT_EQ(view_view.size(), arr.size());
}
