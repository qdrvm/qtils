/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <fmt/format.h>
#include <system_error>

namespace boost::system {
  class error_code;
}  // namespace boost::system

template <>
struct fmt::formatter<std::error_code> {
  static constexpr auto parse(format_parse_context &ctx) {
    return ctx.begin();
  }
  static auto format(const auto &error, format_context &ctx) {
    return fmt::format_to(ctx.out(),
        "{}({}) {}",
        error.category().name(),
        error.value(),
        error.message());
  }
};

template <>
struct fmt::formatter<boost::system::error_code>
    : fmt::formatter<std::error_code> {};
