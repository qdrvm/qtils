/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstring>
#include <qtils/bytes.hpp>

namespace qtils {
  inline void append(Bytes &l, BytesIn r) {
    auto offset = l.size();
    l.resize(offset + r.size());
    memcpy(l.data() + offset, r.data(), r.size());
  }
}  // namespace qtils
