/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <fmt/ranges.h>

#include <qtils/span_adl.hpp>

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

  /// Parse format flags (0x, 0X, x, X)
  constexpr auto parse(format_parse_context &ctx) {
    auto it = ctx.begin();
    auto end = [&] { return it == ctx.end() or * it == '}'; };
    if (end()) {
      return it;
    }
    prefix = *it == '0';
    if (prefix) {
      ++it;
    }
    if (not end()) {
      lower = *it == 'x';
      if (lower or *it == 'X') {
        ++it;
        full = true;
        if (end()) {
          return it;
        }
      }
    }

#if FMT_VERSION >= 100000
    fmt::report_error(R"("x"/"X" or "0x"/"0X" expected)");
#else
    fmt::throw_format_error(R"("x"/"X" or "0x"/"0X" expected)");
#endif
  }

  /// Format BytesIn as hex string
  auto format(const qtils::Hex &bytes, format_context &ctx) const {
    auto out = ctx.out();
    if (prefix) {
      out = fmt::detail::write(out, "0x");
    }
    constexpr size_t kHead = 2, kTail = 2, kSmall = 1;
    if (full or bytes.v.v.size() <= kHead + kTail + kSmall) {
      return lower ? fmt::format_to(out, "{:02x}", fmt::join(bytes.v.v, ""))
                   : fmt::format_to(out, "{:02X}", fmt::join(bytes.v.v, ""));
    }
    return fmt::format_to(out,
        "{:02x}…{:02x}",
        fmt::join(bytes.v.v.first(kHead), ""),
        fmt::join(bytes.v.v.last(kTail), ""));
  }
};
