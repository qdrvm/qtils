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
  ByteArr<N> array_from_span(ByteView view) {
    QTILS_ASSERT_GREATER_EQ(view.size(), N);
    ByteArr<N> array;
    std::ranges::copy_n(view.begin(), N, array.begin());
    return array;
  }

  /**
   * @brief Create an array from the span content, filling the tail with filler
   * if array size is greater than span size
   */
  template <size_t N>
  ByteArr<N> array_from_span_fill(ByteView view, uint8_t filler = 0) {
    ByteArr<N> array;
    const auto s = std::min(N, view.size());
    std::ranges::copy_n(view.begin(), s, array.begin());
    std::ranges::fill(
        std::ranges::subrange(array.begin() + s, array.end()), filler);
    return array;
  }

}  // namespace qtils
