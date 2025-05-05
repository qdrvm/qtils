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
      return to_hex(Hex{*this});
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

/// Formatter for qtils::ByteView
template <>
struct fmt::formatter<qtils::ByteView> {
  // Presentation format: 's' - short, 'l' - long.
  char presentation = 's';

  // Parses format specifications of the form ['s' | 'l'].
  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it != end && (*it == 's' || *it == 'l')) {
      presentation = *it++;
    }

    // Check if reached the end of the range:
    if (it != end && *it != '}') {
      throw format_error("invalid format");
    }

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the ByteArr using the parsed format specification (presentation)
  // stored in this formatter.
  template <typename FormatContext>
  auto format(const qtils::ByteView &view, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.

    if (view.empty()) {
      static constexpr string_view message("<empty>");
      return std::copy(message.begin(), message.end(), ctx.out());
    }

    if (presentation == 's' && view.size() > 5) {
      return fmt::format_to(ctx.out(),
          "0x{:04x}…{:04x}",
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          htobe16(*reinterpret_cast<const uint16_t *>(view.data())),
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          htobe16(*reinterpret_cast<const uint16_t *>(
              view.data() + view.size() - sizeof(uint16_t))));
    }

    return fmt::format_to(ctx.out(), "0x{}", view.toHex());
  }
};

/// Formatter for span of unsigned ints
template <>
struct fmt::formatter<std::span<const unsigned int>> {
  constexpr auto parse(format_parse_context &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::span<const unsigned int> &span, FormatContext &ctx) {
    auto out = ctx.out();
    return fmt::format_to(out, "[{}]", fmt::join(span, ", "));
  }
};
