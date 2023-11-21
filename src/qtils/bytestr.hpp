/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string_view>

#include <qtils/bytes.hpp>

namespace qtils {
  inline const uint8_t *str2byte(const char *s) {
    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<const uint8_t *>(s);
  }

  inline const char *byte2str(const uint8_t *s) {
    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<const char *>(s);
  }

  inline uint8_t *str2byte(char *s) {
    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<uint8_t *>(s);
  }

  inline char *byte2str(uint8_t *s) {
    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<char *>(s);
  }

  inline BytesIn str2byte(std::span<const char> s) {
    return {str2byte(s.data()), s.size()};
  }

  inline std::string_view byte2str(const BytesIn &s) {
    return {byte2str(s.data()), s.size()};
  }

  inline BytesOut str2byte(std::span<char> s) {
    return {str2byte(s.data()), s.size()};
  }
}  // namespace qtils
