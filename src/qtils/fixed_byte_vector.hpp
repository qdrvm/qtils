/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/assert.hpp>
#include <qtils/bytes.hpp>

namespace qtils {
 
  /**
   * Fixed-capacity vector meant to be allocated on stack
   * Used to avoid heap allocations for small vectors with known capacity
   * @tparam N - maximum capacity
   */
  template <size_t N>
  struct FixedByteVector {
    ByteArray<N> data;
    size_t size;

    FixedByteVector() = default;

    FixedByteVector(ByteSpan span) : size{span.size()} {
      QTILS_ASSERT_LESS_EQ(span.size(), N);
      std::copy_n(span.begin(), N, data.begin());
    }

    ByteSpan span() const & {
      return ByteSpan{data.data(), size};
    }

    ByteSpan span() const && = delete;
  };
}
