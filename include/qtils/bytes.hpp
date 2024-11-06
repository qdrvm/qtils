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


  template <std::unsigned_integral T = const unsigned char>
  struct BitSpan {
    std::span<T> bytes;
    size_t start_bit{};
    size_t end_bit = bytes.size_bytes() * 8;

    explicit BitSpan() : bytes{}, start_bit{0}, end_bit{0} {}

    explicit BitSpan(std::span<T> s)
        : bytes{s}, start_bit{0}, end_bit{s.size_bytes() * 8} {}

    explicit BitSpan(std::span<T> s, size_t start_bit, size_t end_bit)
        : bytes{s}, start_bit{start_bit}, end_bit{end_bit} {}

    BitSpan(const BitSpan<T> &s)
        : bytes{s.bytes}, start_bit{s.start_bit}, end_bit{s.end_bit} {}

    template <typename U = T>
      requires(std::is_const_v<U>)
    BitSpan(const BitSpan<std::remove_const_t<U>> &s)
        : bytes{s.bytes}, start_bit{s.start_bit}, end_bit{s.end_bit} {}

    BitSpan &operator=(const BitSpan<T> &s) {
      bytes = s.bytes;
      start_bit = s.start_bit;
      end_bit = s.end_bit;
      return *this;
    }

    template <typename U = T>
      requires(std::is_const_v<U>)
    BitSpan &operator=(const BitSpan<std::remove_const_t<U>> &s) {
      bytes = s.bytes;
      start_bit = s.start_bit;
      end_bit = s.end_bit;
      return *this;
    }

    uint8_t operator[](size_t bit_idx) const {
      QTILS_ASSERT_LESS(start_bit + bit_idx, end_bit);
      return (bytes[(start_bit + bit_idx) / 8] >> ((start_bit + bit_idx) % 8))
           & 0x1;
    }

    uint8_t get_as_byte(size_t offset_bits, size_t len_bits) const {
      size_t start = start_bit + offset_bits;
      size_t end = start_bit + offset_bits + len_bits - 1;

      QTILS_ASSERT_LESS_EQ(end, end_bit);
      QTILS_ASSERT_LESS_EQ(len_bits, 8);
      size_t first_byte = start / 8;
      size_t second_byte = end / 8;
      if (first_byte == second_byte) {
        return (bytes[first_byte] >> (start % 8)) & ((1 << len_bits) - 1);
      }
      return ((bytes[first_byte] >> (start % 8))
              | (bytes[second_byte] << (8 - start % 8)))
           & ((1 << len_bits) - 1);
    }

    template <typename U = T>
      requires(!std::is_const_v<U>)
    void set_bit(size_t bit_idx, T bit) {
      uint8_t &byte = bytes[(start_bit + bit_idx) / 8];
      uint8_t mask = (1 << ((start_bit + bit_idx) % 8));
      byte = (byte & ~mask) | (-bit & mask);
    }

    struct const_iterator {
      const BitSpan *span;
      size_t current_bit;

      using value_type = uint8_t;
      using difference_type = ssize_t;

      const_iterator &operator++() {
        current_bit++;
        return *this;
      }

      const_iterator operator++(int) {
        const_iterator copy{*this};
        current_bit++;
        return copy;
      }

      uint8_t operator*() const {
        return (*span)[current_bit];
      }

      bool operator==(const const_iterator &other) const {
        return span->bytes.data() + span->start_bit + current_bit
            == other.span->bytes.data() + +other.span->start_bit
                   + other.current_bit;
      }
    };
    static_assert(std::input_or_output_iterator<const_iterator>);

    const_iterator begin() const {
      return {this, 0};
    }

    const_iterator end() const {
      return {this, size_bits()};
    }

    size_t size_bits() const {
      return end_bit - start_bit;
    }

    BitSpan<const T> subspan(size_t offset, size_t length) const {
      return BitSpan<const T>{
          bytes, start_bit + offset, start_bit + offset + length};
    }

    BitSpan<T> subspan(size_t offset, size_t length) {
      return BitSpan<T>{bytes, start_bit + offset, start_bit + offset + length};
    }

    BitSpan<const T> skip_first(size_t offset) const {
      return BitSpan<T>{
          bytes, start_bit + offset, start_bit + size_bits() - offset};
    }

    BitSpan<T> skip_first(size_t n) {
      QTILS_ASSERT_LESS_EQ(start_bit + n, end_bit);
      return BitSpan<T>{bytes, start_bit + n, end_bit};
    }
  };
  static_assert(std::ranges::range<BitSpan<>>);
  static_assert(std::ranges::range<BitSpan<unsigned char>>);


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
  BytesN<N> array_from_span_fill(ByteSpan span, unsigned char filler = 0) {
    BytesN<N> array;
    const auto s = std::min(N, span.size());
    std::ranges::copy_n(span.begin(), s, array.begin());
    std::ranges::fill(std::ranges::subrange(array.begin() + s, array.end()),
                      filler);
    return array;
  }
}  // namespace qtils


template <typename T>
struct std::formatter<qtils::BitSpan<T>, char> {
  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) {
    auto it = ctx.begin();
    return it;
  }

  template <class FmtContext>
  FmtContext::iterator format(qtils::BitSpan<T> s, FmtContext &ctx) const {
    auto out = ctx.out();
    for (auto bit : s) {
      *out = bit ? '1' : '0';
      ++out;
    }
    return out;
  }
};

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
  FmtContext::iterator format(const qtils::Bytes &v,
                              FmtContext &ctx) const {
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
  FmtContext::iterator format(const qtils::BytesN<N> &s,
                              FmtContext &ctx) const {
    auto out = ctx.out();
    std::format_to(out, "{}", qtils::ByteSpan{s});
    return out;
  }
};
