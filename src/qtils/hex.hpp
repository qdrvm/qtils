/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <fmt/compile.h>
#include <fmt/ranges.h>
#include <qtils/span_adl.hpp>
#include <type_traits>

namespace qtils {

  /**
   * @struct Hex
   * @brief Wrapper for hex encoding of byte sequences.
   */
  struct Hex {
    SpanAdl<const uint8_t> v;  ///< Underlying byte sequence to be hex-encoded
  };

  /**
   * @brief Convert byte container wrapped in Hex to hexadecimal string.
   * @param data Hex wrapper around BytesIn
   * @return Uppercase hex-encoded string (2 chars per byte)
   */
  inline std::string to_hex(const Hex &data) {
    static constexpr char hex_chars[] = "0123456789ABCDEF";

    std::string result;
    result.resize(data.v.v.size() * 2);

    auto it = result.begin();
    for (uint8_t byte : data.v.v) {
      *it++ = hex_chars[byte >> 4];
      *it++ = hex_chars[byte & 0x0F];
    }

    return result;
  }

}  // namespace qtils

/// Formatter for BytesIn with support for 0x/0X prefix and short/long modes
template <>
struct fmt::formatter<qtils::Hex> {
  bool prefix = true;  ///< Show prefix (0x or 0X)
  bool full = false;   ///< Show full content or abbreviated form
  bool lower = true;   ///< Use lowercase hex digits

  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    constexpr bool is_compile =
        std::is_same_v<ParseContext, fmt::detail::compile_parse_context<char>>;

    auto it = ctx.begin();
    auto end = ctx.end();

    if (it == end || *it == '}') {
      return it;
    }

    if (*it == '0') {
      prefix = true;
      ++it;
    }

    if (it != end && (*it == 'x' || *it == 'X')) {
      lower = (*it == 'x');
      full = true;
      ++it;
    } else if constexpr (!is_compile) {
      throw fmt::format_error("qtils::Hex: expected 'x' or 'X'");
    }
    return it;
  }

  /// Format BytesIn as hex string
  auto format(const std::span<const uint8_t> &span, format_context &ctx) const {
    auto out = ctx.out();
    if (prefix) {
      out = fmt::detail::write(out, "0x");
    }
    constexpr size_t kHead = 2, kTail = 2, kSmall = 1;
    if (full or span.size() <= kHead + kTail + kSmall) {
      return lower ? fmt::format_to(out, "{:02x}", fmt::join(span, ""))
                   : fmt::format_to(out, "{:02X}", fmt::join(span, ""));
    }
    return fmt::format_to(out,
        "{:02x}…{:02x}",
        fmt::join(span.first(kHead), ""),
        fmt::join(span.last(kTail), ""));
  }
};
