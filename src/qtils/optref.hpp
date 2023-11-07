/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <optional>

#include <qtils/move.hpp>

namespace qtils {
  /**
   * - `T *`
   * - `std::optional<std::reference_wrapper<T>>`
   */
  template <typename T>
  class OptRef {
   public:
    OptRef() = default;
    OptRef(T *ptr) : ptr_{ptr} {}
    template <typename U>
    OptRef(U other)
      requires std::is_const_v<T>
        and std::is_same_v<U, OptRef<std::remove_const_t<T>>>
        : ptr_{std::move(other.ptr_.v)} {}

    operator bool() const {
      return ptr_.v != nullptr;
    }
    T &operator*() const {
      if (ptr_.v == nullptr) {
        throw std::bad_optional_access{};
      }
      return *ptr_.v;
    }
    T *operator->() const {
      if (ptr_.v == nullptr) {
        throw std::bad_optional_access{};
      }
      return ptr_.v;
    }

   protected:
    MoveCopy<T *> ptr_;

    template <typename U>
    friend class OptRef;
  };
}  // namespace qtils
