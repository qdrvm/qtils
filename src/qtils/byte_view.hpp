/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/cxx20/lexicographical_compare_three_way.hpp>

#include <ostream>
#include <span>

#include <qtils/endianness_macros.hpp>
#include <qtils/hex.hpp>

namespace qtils {

  template <size_t MaxSize, typename Allocator>
  class SLBuffer;

  /**
   * @class ByteView
   * @brief A lightweight read-only view over a sequence of bytes.
   *
   * Provides convenience methods for accessing and manipulating
   * spans of immutable bytes, with support for conversion to hex,
   * comparison and reinterpretation as strings.
   */
  class ByteView : public std::span<const uint8_t> {
   public:
    using span::span;

    /// Deleted constructor for initializer list rvalue
    ByteView(std::initializer_list<uint8_t> &&) = delete;

    /// Implicit conversion from span
    ByteView(const span &other) : span(other) {}

    /// Conversion constructor from a span of integral bytes
    template <typename T>
      requires std::is_integral_v<std::decay_t<T>> and (sizeof(T) == 1)
    ByteView(std::span<T> other)
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        : span(reinterpret_cast<const uint8_t *>(other.data()), other.size()) {}

    /// Assignment from any span-like value
    template <typename T>
    ByteView &operator=(T &&value) {
      span::operator=(std::forward<T>(value));
      return *this;
    }

    /// Drop first N bytes (template version)
    template <size_t count>
    void dropFirst() {
      *this = subspan<count>();
    }

    /// Drop first N bytes (runtime version)
    void dropFirst(size_t count) {
      *this = subspan(count);
    }

    /// Drop last N bytes (template version)
    template <size_t count>
    void dropLast() {
      *this = first(size() - count);
    }

    /// Drop last N bytes (runtime version)
    void dropLast(size_t count) {
      *this = first(size() - count);
    }

    /// Convert view content to hexadecimal string
    [[nodiscard]] std::string toHex() const {
      return fmt::format("{:xx}", Hex(*this));
    }

    /// Reinterpret content as string view
    [[nodiscard]] std::string_view toStringView() const {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      return {reinterpret_cast<const char *>(data()), size()};
    }

    /// 3-way comparison operator
    auto operator<=>(const ByteView &other) const {
      return qtils::cxx20::lexicographical_compare_three_way(
          span::begin(), span::end(), other.begin(), other.end());
    }

    /// Equality comparison operator
    auto operator==(const ByteView &other) const {
      return (*this <=> other) == std::strong_ordering::equal;
    }
  };

  /// Output hex representation to ostream
  inline std::ostream &operator<<(std::ostream &os, ByteView view) {
    static constexpr char hex_chars[] = "0123456789ABCDEF";
    for (uint8_t byte : view) {
      os.put(hex_chars[byte >> 4]);
      os.put(hex_chars[byte & 0x0F]);
    }
    return os;
  }

  /**
   * @brief Check if one range starts with another
   * @tparam Super Supersequence type
   * @tparam Prefix Prefix sequence type
   * @param super Container to check
   * @param prefix Expected prefix
   * @return true if prefix matches the beginning of super
   */
  template <typename Super, typename Prefix>
  bool startsWith(const Super &super, const Prefix &prefix) {
    if (std::size(super) >= std::size(prefix)) {
      return std::equal(
          std::begin(prefix), std::end(prefix), std::begin(super));
    }
    return false;
  }

}  // namespace qtils

template <typename Char>
struct fmt::range_format_kind<qtils::ByteView, Char>
    : std::integral_constant<fmt::range_format, fmt::range_format::disabled> {};

/// fmt::formatter specialization for ByteView
template <>
struct fmt::formatter<qtils::ByteView> : fmt::formatter<qtils::Hex> {};
