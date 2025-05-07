/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <boost/algorithm/hex.hpp>

#include <qtils/enum_error_code.hpp>
#include <qtils/outcome.hpp>

/// Maximum supported size for unhexed input (in bytes)
constexpr size_t MAX_UNHEX_SIZE = 64 * 1024 * 1024;

namespace qtils {

  /**
   * @enum UnhexError
   * @brief Error codes related to hex decoding
   */
  enum class UnhexError {
    UNEXPECTED_0X,        ///< String should not begin with "0x"
    REQUIRED_0X,          ///< Expected prefix "0x"
    ODD_LENGTH,           ///< String length is not divisible by 2
    TOO_SHORT,            ///< Target buffer too long for input
    TOO_LONG,             ///< Input too long for target buffer
    FIXED_SIZE_TOO_LONG,  ///< Would exceed max size
    NON_HEX               ///< Contains non-hexadecimal characters
  };

  /// Error string representation for UnhexError
  Q_ENUM_ERROR_CODE(UnhexError) {
    using E = decltype(e);
    switch (e) {
      case E::UNEXPECTED_0X:
        return "String should not begin with \"0x\"";
      case E::REQUIRED_0X:
        return "Expected prefix \"0x\"";
      case E::ODD_LENGTH:
        return "String length is not divisible by 2";
      case E::TOO_SHORT:
        return "Target buffer too long for input";
      case E::TOO_LONG:
        return "Input too long for target buffer";
      case E::FIXED_SIZE_TOO_LONG:
        return "Would exceed max size";
      case E::NON_HEX:
        return "Contains non-hexadecimal characters";
    }
    abort();
  }

  /**
   * @brief Computes number of bytes in unhexed form
   * @param s hex string, possibly prefixed with "0x"
   * @return decoded byte count
   */
  inline size_t unhexSize(std::string_view s) {
    if (s.starts_with("0x")) {
      s.remove_prefix(2);
    }
    return s.size() / 2;
  }

  /**
   * @brief Unhex string into pre-allocated or resizable container
   * @tparam T Output container type (e.g. std::vector or std::array)
   * @param t destination container
   * @param s hex string (without 0x prefix)
   * @param max_size optional size limit for dynamic containers
   * @return success or appropriate UnhexError
   */
  template <typename T>
  outcome::result<void> unhex(
      T &t, std::string_view s, size_t max_size = MAX_UNHEX_SIZE) {
    if (s.starts_with("0x")) {
      return UnhexError::UNEXPECTED_0X;
    }
    if (s.size() % 2 != 0) {
      return UnhexError::ODD_LENGTH;
    }
    const auto count = unhexSize(s);
    if constexpr (requires { t.resize(size_t{}); }) {
      if (count > max_size) {
        return UnhexError::FIXED_SIZE_TOO_LONG;
      }
      try {
        t.resize(count);
      } catch (const std::bad_alloc &) {
        return UnhexError::TOO_LONG;
      }
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

  /**
   * @brief Unhex a hex string to a container
   * @tparam T container type (defaults to Bytes)
   * @param s hex string
   * @return Result with decoded container or error
   */
  template <typename T>
  outcome::result<T> unhex(std::string_view s) {
    T t;
    OUTCOME_TRY(unhex(t, s));
    return t;
  }

  /**
   * @brief Unhex hex string with required or optional 0x prefix
   * @tparam T destination container
   * @param t output container
   * @param s input string (with or without 0x)
   * @param optional_0x whether prefix is optional
   * @return success or error
   */
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

  /**
   * @brief Unhex string with optional 0x prefix into container
   * @tparam T container type (defaults to Bytes)
   * @param s hex string
   * @return Result with decoded container or error
   */
  template <typename T>
  outcome::result<T> unhex0x(std::string_view s) {
    T t;
    OUTCOME_TRY(unhex0x(t, s));
    return t;
  }

}  // namespace qtils
