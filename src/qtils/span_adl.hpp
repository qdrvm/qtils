/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <qtils/cxx20/lexicographical_compare_three_way.hpp>
#include <span>

/**
 * @brief Adapter struct to enable comparison of std::span via ADL.
 *
 * Since std::span does not define its own comparison operators,
 * this wrapper is used to define ordering and equality semantics
 * for span-like objects in a consistent and extensible way.
 *
 * @tparam T Element type of the span
 */
template <typename T>
struct SpanAdl {
  std::span<T> v;  ///< Wrapped span instance
};

/// Deduction guide for SpanAdl
template <typename T>
SpanAdl(T &&t) -> SpanAdl<typename decltype(std::span{t})::element_type>;

/**
 * @brief 3-way comparison operator for SpanAdl vs span-like object
 *
 * Performs byte-wise comparison for byte spans, or lexicographical
 * comparison otherwise.
 *
 * @tparam T Element type of the span
 * @param lhs Left-hand SpanAdl
 * @param rhs Right-hand span-like object
 * @return std::strong_ordering, std::partial_ordering or std::weak_ordering
 */
template <typename T>
auto operator<=>(const SpanAdl<T> &lhs, const auto &rhs)
  requires(requires { std::span<const T>{rhs}; })
{
  auto &[l] = lhs;
  std::span r{rhs};
  if constexpr (std::is_same_v<std::remove_cvref_t<T>, uint8_t>) {
    auto n = std::min(l.size(), r.size());
    auto c = std::memcmp(l.data(), r.data(), n) <=> 0;
    return c != 0 ? c : l.size() <=> r.size();
  }
  return qtils::cxx20::lexicographical_compare_three_way(
      l.begin(), l.end(), r.begin(), r.end());
}

/**
 * @brief Equality operator for SpanAdl vs span-like object
 *
 * Uses 3-way comparison to check full equality.
 *
 * @tparam T Element type of the span
 * @param lhs Left-hand SpanAdl
 * @param rhs Right-hand span-like object
 * @return true if spans are equal in size and content
 */
template <typename T>
bool operator==(const SpanAdl<T> &lhs, const auto &rhs)
  requires(requires { std::span<const T>{rhs}; })
{
  std::span r{rhs};
  return lhs.v.size() == r.size() and (lhs <=> r) == 0;
}
