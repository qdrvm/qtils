/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>

namespace qtils::cxx23::ranges {
  auto contains(auto &&r, const auto &v) {
    return std::ranges::find(r, v) != std::ranges::end(r);
  }

  auto contains_if(auto &&r, const auto &f) {
    return std::ranges::find_if(r, f) != std::ranges::end(r);
  }
}  // namespace qtils::cxx23::ranges
