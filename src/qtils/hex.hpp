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
  using BytesIn = std::span<const uint8_t>;

  /**
   * @struct Hex
   * @brief Wrapper for hex encoding of byte sequences.
   */
  struct Hex : BytesIn {
    using BytesIn::span;

    Hex(BytesIn span) : BytesIn{span} {}
  };

}  // namespace qtils

template <typename Char>
struct fmt::range_format_kind<qtils::Hex, Char>
    : std::integral_constant<fmt::range_format, fmt::range_format::disabled> {};

/**
 * Formatter for BytesIn with various format specifiers:
 *       - {}      : default formatting, abbreviated hexadecimal with ellipsis
 *       - {:x}    : lowercase abbreviated hex
 *       - {:xx}   : full lowercase hex
 *       - {:X}    : uppercase abbreviated hex
 *       - {:XX}   : full uppercase hex
 *       - {:0}    : default abbreviated with "0x" prefix
 *       - {:0x}   : lowercase abbreviated with "0x" prefix
 *       - {:0xx}  : full lowercase hex with "0x" prefix
 *       - {:0X}   : uppercase abbreviated with "0x" prefix
 *       - {:0XX}  : full uppercase hex with "0x" prefix
 */
template <>
struct fmt::formatter<qtils::Hex> {
  /// Whether to prepend the "0x" prefix
  bool prefix = false;
  /// Whether to print a full byte sequence or abbreviated form
  bool full = false;
  /// Use lowercase (true) or uppercase (false) hex digits
  bool lower = true;

  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    constexpr bool is_compile =
        std::is_same_v<ParseContext, detail::compile_parse_context<char>>;

    auto it = ctx.begin();
    auto end = ctx.end();

    if (it == end or *it == '}') {
      return it;
    }

    if (*it == '0') {
      prefix = true;
      ++it;
    }

    if (it == end or *it == '}') {
      return it;
    }

    if (*it == 'x' or *it == 'X') {
      lower = (*it == 'x');
      ++it;
      if (it == end or *it == '}') {
        return it;
      }
      if (*it == 'x' and lower or *it == 'X' and not lower) {
        full = true;
        ++it;
      }
    }

    if (it == end or *it == '}') {
      return it;
    }

    if constexpr (is_compile) {
      report_error("invalid format specifier: expected [0][x|xx|X|XX]");
    } else {
      throw format_error(
          "invalid format specifier for Hex: expected [0][x|xx|X|XX]");
    }
  }

  /// Format BytesIn as hex string
  auto format(const std::span<const uint8_t> &span, format_context &ctx) const {
    auto out = ctx.out();
    if (span.empty()) {
      return detail::write(out, prefix ? "0x" : "<empty>");
    }
    if (prefix) {
      out = detail::write(out, "0x");
    }
    constexpr size_t kHead = 2;
    constexpr size_t kTail = 2;
    constexpr size_t kSmall = 1;

    auto write_bytes = [&](std::span<const uint8_t> bytes) {
      if (lower) {
        for (uint8_t byte : bytes) {
          out = format_to(out, "{:02x}", byte);
        }
      } else {
        for (uint8_t byte : bytes) {
          out = format_to(out, "{:02X}", byte);
        }
      }
    };

    if (full or span.size() < kHead + kTail + kSmall) {
      write_bytes(span);
    } else {
      write_bytes(span.first(kHead));
      out = detail::write(out, "…");
      write_bytes(span.last(kTail));
    }

    return out;
  }
};
