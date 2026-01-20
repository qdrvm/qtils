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
 * Formatter for qtils::Hex.
 *
 * Legacy-compatible custom specifiers:
 *   {}        : abbreviated hex
 *   {:x}      : abbreviated lowercase hex
 *   {:xx}     : full lowercase hex
 *   {:X}      : abbreviated uppercase hex
 *   {:XX}     : full uppercase hex
 *   {:0}      : abbreviated hex with "0x" prefix
 *   {:0x}     : abbreviated lowercase hex with "0x"
 *   {:0xx}    : full lowercase hex with "0x"
 *   {:0X}     : abbreviated uppercase hex with "0x"
 *   {:0XX}    : full uppercase hex with "0x"
 *
 * Optional fmt-like alignment and width (width may be dynamic):
 *   {:<20}          : left align (default)
 *   {:^20}          : center
 *   {:>20}          : right
 *   {:<{};0xx}      : runtime width + custom hex part
 *   {:^{} }         : runtime width, no custom part
 *
 * IMPORTANT:
 *   - Static width MUST NOT start with '0' (otherwise '0' is treated as custom
 *     prefix flag).
 *   - Width/padding is computed in bytes of the produced UTF-8 output. This
 *     preserves legacy semantics.
 */
template <>
struct fmt::formatter<qtils::Hex> {
  // ---- Custom hex options (legacy-compatible) ----
  bool prefix = false;
  bool full = false;
  bool lower = true;

  // ---- Optional alignment/width ----
  char fill = ' ';
  char align = '<';  // default: left
  int width = 0;

  // Dynamic width support: "{:<{}}" or "{:<{0}}"
  bool dynamic_width = false;
  int width_arg_id = -1;

  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    constexpr bool is_compile =
        std::is_same_v<ParseContext, fmt::detail::compile_parse_context<char>>;

    auto it = ctx.begin();
    auto end = ctx.end();

    auto fail = [&] {
      if constexpr (is_compile) {
        report_error(
            "invalid format for Hex: optional [fill][align][width(no leading 0)"
            " | {arg}][;] then optional [0][x|xx|X|XX]");
      } else {
        throw format_error(
            "invalid format for Hex: optional [fill][align][width(no leading 0)"
            " | {arg}][;] then optional [0][x|xx|X|XX]");
      }
    };

    auto is_align = [](char c) { return c == '<' || c == '>' || c == '^'; };
    auto is_digit = [](char c) { return c >= '0' && c <= '9'; };

    // ---- Optional [fill][align] ----
    if (it != end && *it != '}' && (it + 1) != end && it[1] != '}'
        && is_align(it[1])) {
      fill = *it;
      align = it[1];
      it += 2;
    } else if (it != end && *it != '}' && is_align(*it)) {
      align = *it;
      ++it;
    }

    // ---- Optional width ----
    //
    // Static width is parsed only if it starts with [1-9].
    // If it starts with '0', it is NOT width and belongs to custom part.
    if (it != end && *it >= '1' && *it <= '9') {
      int w = 0;
      while (it != end && is_digit(*it)) {
        w = w * 10 + (*it - '0');
        ++it;
      }
      width = w;
    } else if (it != end && *it == '{') {
      // Dynamic width: "{}" or "{n}"
      ++it;  // skip '{'
      if (it == end) {
        fail();
      }

      dynamic_width = true;

      if (*it == '}') {
        // "{}" -> next automatic argument index
        width_arg_id = ctx.next_arg_id();
        ++it;  // skip '}'
      } else {
        // "{n}" -> explicit argument index
        int id = 0;
        if (!is_digit(*it)) {
          fail();
        }
        while (it != end && is_digit(*it)) {
          id = id * 10 + (*it - '0');
          ++it;
        }
        if (it == end || *it != '}') {
          fail();
        }
        ctx.check_arg_id(id);
        width_arg_id = id;
        ++it;  // skip '}'
      }
    }

    // Optional separator between fmt-part and custom-part
    if (it != end && *it == ';') {
      ++it;
    }

    // ---- Optional custom hex part ----
    if (it == end || *it == '}') {
      return it;
    }

    // Legacy: standalone '0' means "prefix only" and is valid.
    if (*it == '0') {
      prefix = true;
      ++it;
      if (it == end || *it == '}') {
        return it;
      }
    }

    if (*it == 'x' || *it == 'X') {
      lower = (*it == 'x');
      ++it;

      if (it == end || *it == '}') {
        return it;  // abbreviated
      }

      if ((*it == 'x' && lower) || (*it == 'X' && !lower)) {
        full = true;
        ++it;
        return it;  // full
      }

      fail();
    }

    fail();
    return it;
  }

  auto format(const std::span<const uint8_t> &span, format_context &ctx) const {
    auto out = ctx.out();

    // Resolve runtime width if dynamic width was used.
    int w = width;
    if (dynamic_width) {
      auto arg = ctx.arg(width_arg_id);

      int dyn = 0;
      bool ok = false;

      // New fmt API: visit through the arg object (visit_format_arg is
      // deprecated).
      arg.visit([&](auto &&v) {
        using T = std::remove_cvref_t<decltype(v)>;
        if constexpr (std::is_integral_v<T>) {
          dyn = static_cast<int>(v);
          ok = true;
        }
      });

      if (!ok) {
        throw format_error("Hex: dynamic width argument is not an integer");
      }

      // Keep behavior simple: negative/zero => no padding.
      w = (dyn > 0) ? dyn : 0;
    }

    // UTF-8 ellipsis. Padding is computed in bytes (legacy semantics).
    static constexpr char kEllipsis[] = u8"…";
    static constexpr int kEllipsisBytes =
        static_cast<int>(sizeof(kEllipsis) - 1);

    constexpr size_t kHead = 2;
    constexpr size_t kTail = 2;
    constexpr size_t kSmall = 1;

    auto write_literal = [](auto it, const char *s) {
      while (*s) {
        *it++ = *s++;
      }
      return it;
    };

    auto write_n_fill = [&](int n) {
      for (int i = 0; i < n; ++i) {
        *out++ = fill;
      }
    };

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

    // Compute the produced output length (in bytes), including prefix and
    // ellipsis.
    auto content_len_bytes = [&]() -> int {
      if (span.empty()) {
        return prefix ? 2 : 7;  // "0x" or "<empty>"
      }

      int len = 0;
      if (prefix) {
        len += 2;  // "0x"
      }

      const bool use_full = full || span.size() < kHead + kTail + kSmall;
      if (use_full) {
        len += static_cast<int>(span.size() * 2);
      } else {
        len += static_cast<int>(kHead * 2);
        len += kEllipsisBytes;
        len += static_cast<int>(kTail * 2);
      }
      return len;
    };

    const int len = content_len_bytes();
    const int pad = (w > len) ? (w - len) : 0;

    auto write_content = [&] {
      if (span.empty()) {
        if (prefix) {
          out = write_literal(out, "0x");
        } else {
          out = write_literal(out, "<empty>");
        }
        return;
      }

      if (prefix) {
        out = write_literal(out, "0x");
      }

      if (full || span.size() < kHead + kTail + kSmall) {
        write_bytes(span);
      } else {
        write_bytes(span.first(kHead));
        for (int i = 0; i < kEllipsisBytes; ++i) {
          *out++ = kEllipsis[i];
        }
        write_bytes(span.last(kTail));
      }
    };

    // Default (and most common) case: left alignment.
    if (align == '<') {
      write_content();
      if (pad > 0) {
        write_n_fill(pad);
      }
      return out;
    }

    if (align == '>') {
      if (pad > 0) {
        write_n_fill(pad);
      }
      write_content();
      return out;
    }

    // Center alignment.
    if (pad <= 0) {
      write_content();
      return out;
    }

    const int left = pad / 2;
    const int right = pad - left;

    write_n_fill(left);
    write_content();
    write_n_fill(right);
    return out;
  }
};
