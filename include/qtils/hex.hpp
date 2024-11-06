/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <fmt/ranges.h>
#include <qtils/bytes.hpp>

namespace qtils {
  struct Hex {
    qtils::BytesIn v;
  };
}  // namespace qtils

template <>
struct fmt::formatter<qtils::BytesIn> {
  bool prefix = true;
  bool full = false;
  bool lower = true;
  constexpr auto parse(format_parse_context &ctx) {
    auto it = ctx.begin();
    auto end = [&] { return it == ctx.end() or *it == '}'; };
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
    fmt::throw_format_error("\"x\"/\"X\" or \"0x\"/\"0X\" expected");
  }
  auto format(const qtils::BytesIn &bytes, format_context &ctx) const {
    auto out = ctx.out();
    if (prefix) {
      out = fmt::detail::write(out, "0x");
    }
    constexpr size_t kHead = 2, kTail = 2, kSmall = 1;
    if (full or bytes.size() <= kHead + kTail + kSmall) {
      return lower ? fmt::format_to(out, "{:02x}", fmt::join(bytes, ""))
                   : fmt::format_to(out, "{:02X}", fmt::join(bytes, ""));
    }
    return fmt::format_to(out,
        "{:02x}…{:02x}",
        fmt::join(bytes.first(kHead), ""),
        fmt::join(bytes.last(kTail), ""));
  }
};
template <>
struct fmt::formatter<qtils::Bytes> : fmt::formatter<qtils::BytesIn> {};
template <>
struct fmt::formatter<qtils::BytesOut> : fmt::formatter<qtils::BytesIn> {};
template <>
struct fmt::formatter<qtils::Hex> : fmt::formatter<qtils::BytesIn> {
  auto format(const qtils::Hex &v, format_context &ctx) const {
    return formatter<qtils::BytesIn>::format(v.v, ctx);
  }
};

// conflicts with `formatter<is_tuple_like>` from <fmt/ranges.h>
template <size_t N>
struct fmt::formatter<qtils::BytesN<N>> {
  formatter() = delete;
};
