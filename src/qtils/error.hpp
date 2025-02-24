/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <system_error>

#include <fmt/format.h>

#include <memory>
#include <string>

#ifdef __has_include
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#define HAS_DEMANGLING
#endif
#endif

namespace boost::system {
  class error_code;
}  // namespace boost::system

namespace qtils {
  std::error_code asError(const auto &e) {
    if constexpr (requires { std::error_code{e}; }) {
      return std::error_code{e};
    } else {
      return make_error_code(e);
    }
  }
}  // namespace qtils

template <>
struct fmt::formatter<std::error_code> {
  // Presentation format:
  // 'l' - long (print category, value, and message)
  // 's' - short (print message only)
  char presentation =
#ifdef FORMAT_ERROR_WITH_FULLTYPE
      'l';
#else
      's';
#endif

  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 's' || *it == 'l')) {
      presentation = *it++;
    }
    if (it != end && *it != '}') {
      throw format_error("invalid format");
    }
    return it;
  }

  [[nodiscard]] auto format(
      const std::error_code &error, format_context &ctx) const {
    if (presentation == 'l') {
      std::string_view category = error.category().name();
#ifdef HAS_DEMANGLING
      int status = 0;
      std::unique_ptr<char, void (*)(void *)> demangled(
          abi::__cxa_demangle(category.data(), nullptr, nullptr, &status),
          std::free);
      if (status == 0) {
        category = demangled.get();
      }
#endif
      return fmt::format_to(
          ctx.out(), "{}({}) {}", category, error.value(), error.message());
    }
    return fmt::format_to(ctx.out(), "{}", error.message());
  }
};

template <>
struct fmt::formatter<boost::system::error_code>
    : fmt::formatter<std::error_code> {};

#undef HAS_DEMANGLING
