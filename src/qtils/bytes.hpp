/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <array>
#include <cstdint>
#include <qtils/byte_arr.hpp>
#include <qtils/byte_vec.hpp>
#include <span>

namespace qtils {

  // Deprecate obsolete names
  using Bytes [[deprecated("Use qtils::ByteVec instead")]] = ByteVec;
  using Buffer [[deprecated("Use qtils::ByteVec instead")]] = ByteVec;
  template <size_t N>
  using BytesN [[deprecated("Use qtils::ByteArr instead")]] = ByteArr<N>;
  template <size_t N>
  using Blob [[deprecated("Use qtils::ByteArr instead")]] = ByteArr<N>;
  template <size_t N>
  using ByteArray [[deprecated("Use qtils::ByteArr instead")]] = ByteArr<N>;

  using BufferView [[deprecated("Use qtils::ByteView instead")]] = ByteView;
  using ByteSpan [[deprecated("Use qtils::ByteView instead")]] = ByteView;

  using BytesIn = std::span<const uint8_t>;
  using BytesOut = std::span<uint8_t>;

  template <size_t N>
  using FixedByteSpan = std::span<const unsigned char, N>;

  template <size_t N>
  using FixedByteSpanMut = std::span<unsigned char, N>;

  using ByteSpanMut = std::span<unsigned char>;

  template <typename T>
  concept AsBytes = requires(const T &t) { BytesIn{t}; };
}  // namespace qtils

/// fmt::formatter specialization for BytesIn
template <>
struct fmt::formatter<qtils::BytesIn> : fmt::formatter<qtils::Hex> {};

/// fmt::formatter specialization for BytesOut
template <>
struct fmt::formatter<qtils::BytesOut> : fmt::formatter<qtils::Hex> {};
