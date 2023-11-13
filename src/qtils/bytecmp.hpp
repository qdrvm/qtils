/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstring>
#include <qtils/bytes.hpp>

namespace qtils {
  template <typename T>
  concept AsBytes = requires(T t) { BytesIn{t}; };

  auto operator<=>(const AsBytes auto &l_, const AsBytes auto &r_) {
    BytesIn l{l_}, r{r_};
    auto n = l.size() < r.size() ? l.size() : r.size();
    auto c = std::memcmp(l.data(), r.data(), n) <=> 0;
    return c != 0 ? c : l.size() <=> r.size();
  }
  bool operator==(const AsBytes auto &l_, const AsBytes auto &r_) {
    return (l_ <=> r_) == 0;
  }
}  // namespace qtils

using qtils::operator<=>;
using qtils::operator==;
