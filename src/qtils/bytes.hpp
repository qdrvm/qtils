/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace qtils {
  using Bytes = std::vector<uint8_t>;
  template <size_t N>
  using BytesN = std::array<uint8_t, N>;
  using BytesIn = std::span<const uint8_t>;
  using BytesOut = std::span<uint8_t>;

  template <size_t N>
  using ByteArray = std::array<uint8_t, N>;

  using ByteSpan = std::span<const unsigned char>;

  template <size_t N>
  using FixedByteSpan = std::span<const unsigned char, N>;

  template <size_t N>
  using FixedByteSpanMut = std::span<unsigned char, N>;

  using ByteSpanMut = std::span<unsigned char>;

  template <typename T>
  concept AsBytes = requires(const T &t) { BytesIn{t}; };

  inline Bytes asVec(BytesIn v) {
    return {v.begin(), v.end()};
  }
}  // namespace qtils
