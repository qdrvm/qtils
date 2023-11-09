/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <fmt/format.h>

#include <qtils/bytes.hpp>

template <>
struct fmt::formatter<qtils::BytesIn> {
  bool prefix = true;
  bool full = false;
  constexpr auto parse(format_parse_context &ctx) {
    auto it = ctx.begin();
    auto end = [&] { return it == ctx.end() or *it == '}'; };
    if (end()) {
      return it;
    }
    if (*it == 'x') {
      ++it;
      prefix = false;
      full = true;
      if (end()) {
        return it;
      }
    } else if (*it == '0') {
      ++it;
      prefix = true;
      if (not end() and *it == 'x') {
        ++it;
        full = true;
        if (end()) {
          return it;
        }
      }
    }
    fmt::throw_format_error("\"x\" or \"0x\" expected");
  }
  auto format(const qtils::BytesIn &bytes, format_context &ctx) const {
    auto out = ctx.out();
    if (prefix) {
      out = fmt::detail::write(out, "0x");
    }
    constexpr size_t kHead = 2, kTail = 2, kSmall = 1;
    if (full or bytes.size() <= kHead + kTail + kSmall) {
      return fmt::format_to(out, "{:02x}", fmt::join(bytes, ""));
    }
    return fmt::format_to(out,
        "{:02x}…{:02x}",
        fmt::join(bytes.first(kHead), ""),
        fmt::join(bytes.last(kTail), ""));
  }
};
template <>
struct fmt::formatter<qtils::Bytes> : fmt::formatter<qtils::BytesIn> {};
template <size_t N>
struct fmt::formatter<qtils::BytesN<N>> : fmt::formatter<qtils::BytesIn> {};
template <>
struct fmt::formatter<qtils::BytesOut> : fmt::formatter<qtils::BytesIn> {};
