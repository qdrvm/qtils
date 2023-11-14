/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <fmt/format.h>
#include <system_error>

template <>
struct fmt::formatter<std::error_code> {
  static constexpr auto parse(format_parse_context &ctx) {
    return ctx.begin();
  }
  static auto format(const std::error_code &error, format_context &ctx) {
    return fmt::format_to(ctx.out(),
        "{}({}) {}",
        error.category().name(),
        error.value(),
        error.message());
  }
};
