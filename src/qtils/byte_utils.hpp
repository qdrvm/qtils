/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string_view>

#include <qtils/bytes.hpp>

namespace qtils {
  inline const uint8_t *str2byte(const char *s) {
    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<const uint8_t *>(s);
  }

  inline const char *byte2str(const uint8_t *s) {
    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<const char *>(s);
  }

  inline uint8_t *str2byte(char *s) {
    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<uint8_t *>(s);
  }

  inline char *byte2str(uint8_t *s) {
    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<char *>(s);
  }

  inline BytesIn str2byte(std::span<const char> s) {
    return {str2byte(s.data()), s.size()};
  }

  inline std::string_view byte2str(const BytesIn &s) {
    return {byte2str(s.data()), s.size()};
  }

  inline BytesOut str2byte(std::span<char> s) {
    return {str2byte(s.data()), s.size()};
  }


  inline Bytes asVec(BytesIn v) {
    return {v.begin(), v.end()};
  }

  template <size_t N>
  BytesN<N> array_from_span(ByteSpan span) {
    QTILS_ASSERT_GREATER_EQ(span.size(), N);
    BytesN<N> array;
    std::ranges::copy_n(span.begin(), N, array.begin());
    return array;
  }

  /**
   * @brief Create an array from the span content, filling the tail with filler
   * if array size is greater than span size
   */
  template <size_t N>
  BytesN<N> array_from_span_fill(ByteSpan span, uint8_t filler = 0) {
    BytesN<N> array;
    const auto s = std::min(N, span.size());
    std::ranges::copy_n(span.begin(), s, array.begin());
    std::ranges::fill(
        std::ranges::subrange(array.begin() + s, array.end()), filler);
    return array;
  }
  
}  // namespace qtils
