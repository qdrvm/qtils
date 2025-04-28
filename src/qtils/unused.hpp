/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>

#include <qtils/empty.hpp>
#include <qtils/tagged.hpp>

namespace qtils {

  /// Special zero-size-type for some things
  ///  (e.g., dummy types of variant, unsupported or experimental).
  template <size_t N>
  using Unused = Tagged<Empty, NumTag<N>>;

}  // namespace qtils
