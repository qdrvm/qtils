/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <boost/algorithm/hex.hpp>
#include <qtils/bytes.hpp>
#include <qtils/outcome.hpp>

namespace qtils {
  enum class Unhex0x {
    No,
    Maybe,
    Yes,
  };
  template <typename T = Bytes>
  outcome::result<T> unhex(
      std::string_view s, Unhex0x want_prefix = Unhex0x::Maybe) {
    constexpr std::string_view kPrefix{"0x"};
    auto has_prefix = s.starts_with(kPrefix);
    if (has_prefix) {
      if (want_prefix == Unhex0x::No) {
        return Q_ERROR("unhex: \"0x\" prefix not allowed");
      }
      s.remove_prefix(kPrefix.size());
    } else if (want_prefix == Unhex0x::Yes) {
      return Q_ERROR("unhex: \"0x\" prefix required");
    }
    if (s.size() % 2 != 0) {
      return Q_ERROR("unhex: even length string required");
    }
    auto count = s.size() / 2;
    T t;
    if constexpr (requires(T t) { t.resize(size_t{}); }) {
      t.resize(count);
    } else {
      if (count < t.size()) {
        return Q_ERROR("unhex: string too short");
      }
      if (count > t.size()) {
        return Q_ERROR("unhex: string too long");
      }
    }
    try {
      boost::algorithm::unhex(s.begin(), s.end(), t.begin());
    } catch (const boost::algorithm::non_hex_input &) {
      return Q_ERROR("unhex: hex string expected");
    }
    return t;
  }

  inline auto operator""_unhex(const char *c, size_t s) {
    return unhex(std::string_view{c, s}).value();
  }
}  // namespace qtils
