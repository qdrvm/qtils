/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/hex.hpp>
#include <qtils/unhex.hpp>
#include <string_view>
#include <vector>

namespace qtils::literals {
  inline std::vector<uint8_t> operator""_bytes(const char *c, size_t s) {
    std::vector<uint8_t> chars(c, c + s);
    return chars;
  }

  inline std::vector<uint8_t> operator""_unhex(const char *c, size_t s) {
    if (s > 2 and c[0] == '0' and c[1] == 'x') {
      return unhex0x(std::string_view(c, s)).value();
    }
    return unhex(std::string_view(c, s)).value();
  }

  inline std::string operator""_hex(const char *c, size_t s) {
    return to_hex(
        Hex{std::span{reinterpret_cast<const uint8_t *>(c), s}});
  }
}  // namespace qtils::literals
