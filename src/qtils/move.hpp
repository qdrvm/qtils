/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <utility>

namespace qtils {
  /**
   * Used to reset `std::coroutine_handle` on move.
   */
  template <typename T>
  struct MoveOnly {
    MoveOnly() : v{} {};
    MoveOnly(T &&v) : v{std::move(v)} {}
    MoveOnly(const MoveOnly &other) = delete;
    MoveOnly(MoveOnly &&other) : v{std::move(other.v)} {
      other.v = T();
    }
    MoveOnly &operator=(const MoveOnly &other) = delete;
    MoveOnly &operator=(MoveOnly &&other) {
      v = std::move(other.v);
      other.v = T();
      return *this;
    }
    T v;
  };

  /**
   * Used by `OptRef` to reset `T *` on move.
   */
  template <typename T>
  struct MoveCopy {
    MoveCopy() : v{} {};
    MoveCopy(const T &v) : v{v} {}
    MoveCopy(T &&v) : v{std::move(v)} {}
    MoveCopy(const MoveCopy &other) = default;
    MoveCopy(MoveCopy &&other) : v{std::move(other.v)} {
      other.v = T();
    }
    MoveCopy &operator=(const MoveCopy &other) = default;
    MoveCopy &operator=(MoveCopy &&other) {
      v = std::move(other.v);
      other.v = T();
      return *this;
    }
    T v;
  };
}  // namespace qtils
