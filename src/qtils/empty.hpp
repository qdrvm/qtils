/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

namespace qtils {

  /// Special zero-size-type for some things
  /// (e.g. unsupported, experimental or empty).
  struct Empty {
    inline constexpr bool operator==(const Empty &) const {
      return true;
    }
  };

}  // namespace qtils
