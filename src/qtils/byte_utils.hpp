/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/assert.hpp>
#include <qtils/bytes.hpp>

namespace qtils {
  template <size_t N>
  Blob<N> array_from_span(ByteSpan span) {
    QTILS_ASSERT_GREATER_EQ(span.size(), N);
    Blob<N> array;
    std::ranges::copy_n(span.begin(), N, array.begin());
    return array;
  }

  /**
   * @brief Create an array from the span content, filling the tail with filler
   * if array size is greater than span size
   */
  template <size_t N>
  Blob<N> array_from_span_fill(ByteSpan span, uint8_t filler = 0) {
    Blob<N> array;
    const auto s = std::min(N, span.size());
    std::ranges::copy_n(span.begin(), s, array.begin());
    std::ranges::fill(
        std::ranges::subrange(array.begin() + s, array.end()), filler);
    return array;
  }

}  // namespace qtils
