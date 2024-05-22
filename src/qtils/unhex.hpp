/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <boost/algorithm/hex.hpp>
#include <qtils/bytes.hpp>
#include <qtils/enum_error_code.hpp>
#include <qtils/outcome.hpp>

namespace qtils {
  enum class UnhexError {
    UNEXPECTED_0X,
    REQUIRED_0X,
    ODD_LENGTH,
    TOO_SHORT,
    TOO_LONG,
    NON_HEX,
  };
  Q_ENUM_ERROR_CODE(UnhexError) {
    using E = decltype(e);
    switch (e) {
      case E::UNEXPECTED_0X:
        return "UNEXPECTED_0X";
      case E::REQUIRED_0X:
        return "REQUIRED_0X";
      case E::ODD_LENGTH:
        return "ODD_LENGTH";
      case E::TOO_SHORT:
        return "TOO_SHORT";
      case E::TOO_LONG:
        return "TOO_LONG";
      case E::NON_HEX:
        return "NON_HEX";
    }
    abort();
  }

  template <typename T>
  outcome::result<void> unhex(T &t, std::string_view s) {
    if (s.starts_with("0x")) {
      return UnhexError::UNEXPECTED_0X;
    }
    if (s.size() % 2 != 0) {
      return UnhexError::ODD_LENGTH;
    }
    auto count = s.size() / 2;
    if constexpr (requires { t.resize(size_t{}); }) {
      t.resize(count);
    } else {
      if (count < t.size()) {
        return UnhexError::TOO_SHORT;
      }
      if (count > t.size()) {
        return UnhexError::TOO_LONG;
      }
    }
    try {
      boost::algorithm::unhex(s.begin(), s.end(), t.begin());
    } catch (const boost::algorithm::non_hex_input &) {
      return UnhexError::NON_HEX;
    }
    return outcome::success();
  }

  template <typename T = Bytes>
  outcome::result<T> unhex(std::string_view s) {
    T t;
    OUTCOME_TRY(unhex(t, s));
    return t;
  }

  template <typename T>
  outcome::result<void> unhex0x(
      T &t, std::string_view s, bool optional_0x = false) {
    if (s.starts_with("0x")) {
      s.remove_prefix(2);
    } else if (not optional_0x) {
      return UnhexError::REQUIRED_0X;
    }
    return unhex<T>(t, s);
  }

  template <typename T = Bytes>
  outcome::result<T> unhex0x(std::string_view s) {
    T t;
    OUTCOME_TRY(unhex0x(t, s));
    return t;
  }

  inline auto operator""_unhex(const char *c, size_t s) {
    return unhex(std::string_view{c, s}).value();
  }
}  // namespace qtils
