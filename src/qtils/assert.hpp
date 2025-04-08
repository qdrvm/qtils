/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
#include <expected>
#endif

#include <algorithm>
#include <format>
#include <iostream>
#include <print>
#include <source_location>
#include <string_view>

#include <qtils/cxx23/print.hpp>

namespace qtils {

  inline void print_and_abort(std::string_view message,
      std::source_location loc = std::source_location::current()) {
    cxx23::print(std::cerr,
        "{}:{}: assertion failed in {}: {}\n",
        loc.file_name(),
        loc.line(),
        loc.function_name(),
        message);
    std::abort();
  }

  inline void expect(bool condition,
      std::string_view message,
      std::source_location loc = std::source_location::current()) {
    if (!condition) [[unlikely]] {
      print_and_abort(message, loc);
    }
  }

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
  template <typename T, typename E>
  inline void expect(const std::expected<T, E> &expected,
      std::string_view expression,
      std::source_location loc = std::source_location::current()) {
    if (!expected) [[unlikely]] {
      print_and_abort(std::format("'{}' contains an unexpected result '{}'",
                          expression,
                          expected.error()),
          loc);
    }
  }
#endif

  template <typename A, typename B, std::relation<A, B> Cmp>
  void expect_cmp(const A &a,
      const B &b,
      std::string_view sign,
      std::source_location loc = std::source_location::current()) {
    if (!Cmp{}(a, b)) [[unlikely]] {
      print_and_abort(std::format("{} {} {}", a, sign, b), loc);
    }
  }

  template <std::ranges::range A, std::ranges::range B>
  void expect_range_eq(A &&a,
      B &&b,
      std::source_location loc = std::source_location::current()) {
    if (!std::ranges::equal(a, b)) [[unlikely]] {
      print_and_abort(std::format("{} == {}", a, b), loc);
    }
  }
}  // namespace qtils

#ifdef QTILS_ENABLE_ASSERT

#define QTILS_ASSERT(cond) ::qtils::expect(cond, #cond);
#define QTILS_ASSERT_LESS(a, b) \
  ::qtils::expect_cmp<decltype(a), decltype(b), std::less<>>((a), (b), "<");
#define QTILS_ASSERT_LESS_EQ(a, b)                                  \
  ::qtils::expect_cmp<decltype(a), decltype(b), std::less_equal<>>( \
      (a), (b), "<=");
#define QTILS_ASSERT_GREATER(a, b) \
  ::qtils::expect_cmp<decltype(a), decltype(b), std::greater<>>((a), (b), ">");
#define QTILS_ASSERT_GREATER_EQ(a, b)                                  \
  ::qtils::expect_cmp<decltype(a), decltype(b), std::greater_equal<>>( \
      (a), (b), ">=");
#define QTILS_ASSERT_EQ(a, b)                                     \
  ::qtils::expect_cmp<decltype(a), decltype(b), std::equal_to<>>( \
      (a), (b), "==");
#define QTILS_ASSERT_NOT_EQ(a, b)                                     \
  ::qtils::expect_cmp<decltype(a), decltype(b), std::not_equal_to<>>( \
      (a), (b), "!=");
#define QTILS_ASSERT_RANGE_EQ(a, b) ::qtils::expect_range_eq((a), (b));
#define QTILS_ASSERT_HAS_VALUE(a) ::qtils::expect(a, #a);

#else

#define QTILS_ASSERT(cond)
#define QTILS_ASSERT_LESS(a, b)
#define QTILS_ASSERT_LESS_EQ(a, b)
#define QTILS_ASSERT_GREATER(a, b)
#define QTILS_ASSERT_GREATER_EQ(a, b)
#define QTILS_ASSERT_EQ(a, b)
#define QTILS_ASSERT_NOT_EQ(a, b)
#define QTILS_ASSERT_RANGE_EQ(a, b)
#define QTILS_ASSERT_HAS_VALUE(a)

#endif
