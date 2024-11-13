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

#include <qtils/assert.hpp>

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

  inline std::string_view to_string_view(ByteSpan span) {
    return std::string_view{
        reinterpret_cast<const char *>(span.data()), span.size()};
  }

  inline std::string hex(ByteSpan span) {
    std::string s(span.size() * 2, '\0');
    for (size_t i = 0; i < span.size(); i++) {
      std::to_chars(s.data() + 2 * i, s.data() + 2 * i + 1, span[i] >> 4, 16);
      std::to_chars(
          s.data() + 2 * i + 1, s.data() + 2 * i + 2, span[i] & 0xF, 16);
    }
    return s;
  }

  template <size_t N>
  BytesN<N> array_from_span(ByteSpan span) {
    QTILS_ASSERT_GREATER_EQ(span.size(), N);
    BytesN<N> array;
    std::ranges::copy_n(span.begin(), N, array.begin());
    return array;
  }

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

template <>
struct std::formatter<qtils::ByteSpan, char> {
  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) {
    auto it = ctx.begin();
    return it;
  }

  template <class FmtContext>
  FmtContext::iterator format(qtils::ByteSpan s, FmtContext &ctx) const {
    auto out = ctx.out();
    for (auto i : s.subspan(0, s.size() - 1)) {
      std::format_to(out, "{:02x}", i);
    }
    if (!s.empty()) {
      std::format_to(out, "{:02x}", s.back());
    }
    return out;
  }
};

template <>
struct std::formatter<qtils::Bytes, char> {
  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) {
    auto it = ctx.begin();
    return it;
  }

  template <class FmtContext>
  FmtContext::iterator format(const qtils::Bytes &v, FmtContext &ctx) const {
    auto out = ctx.out();
    for (auto i : qtils::ByteSpan{v}.subspan(0, v.size() - 1)) {
      std::format_to(out, "{:02x}", i);
    }
    if (!v.empty()) {
      std::format_to(out, "{:02x}", v.back());
    }
    return out;
  }
};

template <size_t N>
struct std::formatter<qtils::BytesN<N>, char> {
  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) {
    auto it = ctx.begin();
    return it;
  }

  template <class FmtContext>
  FmtContext::iterator format(
      const qtils::BytesN<N> &s, FmtContext &ctx) const {
    auto out = ctx.out();
    std::format_to(out, "{}", qtils::ByteSpan{s});
    return out;
  }
};
