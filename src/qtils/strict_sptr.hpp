/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>

namespace qtils {

  /**
   * @brief A strict version of std::shared_ptr that guarantees non-null values.
   *
   * This class prevents default construction and ensures that the shared
   * pointer is never null. It removes unsafe methods such as `reset()` and
   * `operator=`. Provides safe access to the underlying std::shared_ptr.
   *
   * @tparam T The type of the managed object.
   */
  template <class T>
    requires(not std::is_void_v<T>)
  class StrictSharedPtr : public std::shared_ptr<T> {
   public:
    // Default constructor is deleted to prevent an uninitialized state
    StrictSharedPtr() = delete;

    // Explicitly delete constructor from nullptr
    StrictSharedPtr(std::nullptr_t) = delete;

    /**
     * @brief Constructs a StrictSharedPtr from an existing std::shared_ptr.
     *
     * @param sptr The shared pointer to be wrapped.
     * @throws std::invalid_argument if the pointer is null.
     */
    // NOLINTNEXTLINE(google-explicit-constructor)
    StrictSharedPtr(std::shared_ptr<T> sptr)
        : std::shared_ptr<T>(std::move(sptr)) {
      if (!this->get()) {
        throw std::invalid_argument("Shared pointer is null");
      }
    }

  private:
   struct ForceInject{}; // Auxiliary arg appended to select ctor for DI

  public:
    StrictSharedPtr(std::shared_ptr<T> sptr, ForceInject):
         StrictSharedPtr(std::move(sptr)) {}

    // Copy constructor and assignment
    StrictSharedPtr(const StrictSharedPtr &) = default;
    StrictSharedPtr &operator=(const StrictSharedPtr &) = default;

    // Move constructor
    StrictSharedPtr(StrictSharedPtr &&other) noexcept
        : std::shared_ptr<T>(std::move(other)) {
      if (!this->get()) {
        throw std::invalid_argument("StrictSharedPtr moved to null state");
      }
    }

    // Move assignment
    StrictSharedPtr &operator=(StrictSharedPtr &&other) noexcept {
      std::shared_ptr<T>::operator=(std::move(other));
      if (!this->get()) {
        throw std::invalid_argument("StrictSharedPtr moved to null state");
      }
      return *this;
    }

    // Disable methods that could break the invariant
    void reset() = delete;
    void reset(T *) = delete;
    void reset(T *, std::default_delete<T>) = delete;
    void reset(T *, void (*)(T *)) = delete;
    void reset(T *, void (*)(T *, void *), void *) = delete;

    void swap(std::shared_ptr<T> &) = delete;
    void operator=(std::shared_ptr<T> &) = delete;
    void operator=(std::shared_ptr<T> &&) = delete;

    /**
     * @brief Implicit conversion operator to std::shared_ptr<T>.
     *
     * Allows `StrictSharedPtr<T>` to be used in functions that expect
     * `std::shared_ptr<T>`.
     */
    // NOLINTNEXTLINE(google-explicit-constructor)
    operator std::shared_ptr<T>() const noexcept {
      return *this;
    }
  };

}  // namespace qtils
