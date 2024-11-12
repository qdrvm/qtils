/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/bytes.hpp>

namespace qtils {
  /**
   * Fixed-capacity vector meant to be allocated on stack
   * Used to avoid heap allocations for small vectors with known capacity
   * @tparam N - maximum capacity
   */
  template <size_t N>
  struct FixedByteVector {
    BytesN<N> data;
    size_t size;

    FixedByteVector() : data{}, size{} {}

    FixedByteVector(ByteSpan span) : size{span.size()} {
      QTILS_ASSERT_LESS_EQ(span.size(), N);
      std::copy_n(span.begin(), N, data.begin());
    }

    ByteSpan span() const {
      return ByteSpan{data.data(), size};
    }
  };

  template <size_t N>
  BytesN<N> array_from_span(ByteSpan span) {
    QTILS_ASSERT_GREATER_EQ(span.size(), N);
    BytesN<N> array;
    std::ranges::copy_n(span.begin(), N, array.begin());
    return array;
  }

  template <size_t N>
  BytesN<N> array_from_span_fill(ByteSpan span, unsigned char filler = 0) {
    BytesN<N> array;
    const auto s = std::min(N, span.size());
    std::ranges::copy_n(span.begin(), s, array.begin());
    std::ranges::fill(
        std::ranges::subrange(array.begin() + s, array.end()), filler);
    return array;
  }
}  // namespace qtils
