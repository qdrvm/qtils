/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/byte_arr.hpp>
#include <qtils/byte_vec.hpp>
#include <qtils/hex.hpp>
#include <qtils/unhex.hpp>
#include <string_view>
#include <vector>

namespace qtils::literals {

  /**
   * @brief Converts a string literal into a vector of bytes.
   *
   * Example: auto v = "abc"_bytes; // {'a', 'b', 'c'}
   *
   * @param c character pointer
   * @param s length of the string
   * @return vector of raw characters as bytes
   */
  inline std::vector<uint8_t> operator""_bytes(const char *c, size_t s) {
    std::vector<uint8_t> chars(c, c + s);
    return chars;
  }

  /**
   * @brief Converts a hex string literal into a vector of bytes.
   *
   * Supports optional "0x" prefix.
   * Fails with std::bad_optional_access if the hex string is invalid.
   *
   * Example: auto data = "0xdeadbeef"_unhex;
   *
   * @param c character pointer
   * @param s length of the string
   * @return decoded byte vector
   */
  inline std::vector<uint8_t> operator""_unhex(const char *c, size_t s) {
    if (s > 2 and c[0] == '0' and c[1] == 'x') {
      return unhex0x<std::vector<uint8_t>>(std::string_view(c, s)).value();
    }
    return unhex<std::vector<uint8_t>>(std::string_view(c, s)).value();
  }

  /**
   * @brief Converts a string literal into a hexadecimal string.
   *
   * Encodes the literal into hex representation using uppercase hex digits.
   *
   * Example: auto hex = "abc"_hex; // "616263"
   *
   * @param c character pointer
   * @param s length of the string
   * @return hex-encoded string
   */
  inline std::string operator""_hex(const char *c, size_t s) {
    return fmt::format("{:xx}", Hex(reinterpret_cast<const uint8_t *>(c), s));
  }

  /**
   * @brief Creates a ByteVec from a raw string literal.
   *
   * @details This literal simply copies the raw characters (as bytes)
   * into a ByteVec. It does **not** interpret the content as hexadecimal —
   * for hex decoding, use the `"_unhex"` literal instead.
   *
   * @note The input is taken as-is, including non-ASCII or null bytes.
   * @example
   *   using namespace qtils::literals;
   *   auto v = "abc"_vec; // v = {'a', 'b', 'c'}
   *
   * @param c Pointer to the character data
   * @param s Length of the string literal
   * @return ByteVec containing the raw characters
   */
  inline ByteVec operator""_vec(const char *c, size_t s) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return {c, c + s};
  }

}  // namespace qtils::literals
