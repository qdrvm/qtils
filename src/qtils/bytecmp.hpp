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
    auto c = l.size() <=> r.size();
    return c != 0 ? c : std::memcmp(l.data(), r.data(), l.size()) <=> 0;
  }
}  // namespace qtils

using qtils::operator<=>;
