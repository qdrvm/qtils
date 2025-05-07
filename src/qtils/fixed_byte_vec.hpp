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
  struct FixedByteVec {
    ByteArr<N> data;
    size_t size;

    FixedByteVec() = default;

    FixedByteVec(ByteView view) : size{view.size()} {
      QTILS_ASSERT_LESS_EQ(view.size(), N);
      std::copy_n(view.begin(), N, data.begin());
    }

    ByteView view() const & {
      return ByteView{data.data(), size};
    }

    ByteView view() const && = delete;
  };

}  // namespace qtils
