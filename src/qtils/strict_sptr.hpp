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
  class StrictSharedPtr {
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
    StrictSharedPtr(std::shared_ptr<T> sptr) : ptr_(std::move(sptr)) {
      if (!ptr_) {
        throw std::invalid_argument("Shared pointer is null");
      }
    }

   private:
    struct ForceInject {};  // Auxiliary arg appended to select ctor for DI

   public:
    StrictSharedPtr(std::shared_ptr<T> sptr, ForceInject)
        : StrictSharedPtr(std::move(sptr)) {}

    // Copy constructor and assignment
    StrictSharedPtr(const StrictSharedPtr &) = default;
    StrictSharedPtr &operator=(const StrictSharedPtr &) = default;

    // Move constructor and assignment
    StrictSharedPtr(StrictSharedPtr &&) noexcept = default;
    StrictSharedPtr &operator=(StrictSharedPtr &&) noexcept = default;

    T *operator->() const noexcept {
      return ptr_.get();
    }
    T &operator*() const noexcept {
      return *ptr_;
    }

    /**
     * @brief Implicit conversion operator to std::shared_ptr<T>.
     *
     * Allows `StrictSharedPtr<T>` to be used in functions that expect
     * `std::shared_ptr<T>`.
     */
    // NOLINTNEXTLINE(google-explicit-constructor)
    operator std::shared_ptr<T>() const noexcept {
      return ptr_;
    }

    T *get() const noexcept {
      return ptr_.get();
    }

    size_t use_count() const noexcept {
      return ptr_.use_count();
    }

    template <typename U>
    bool owner_before(const StrictSharedPtr<U> &other) const noexcept {
      return ptr_.owner_before(other.ptr_);
    }

    template <typename U>
    bool owner_before(const std::shared_ptr<U> &other) const noexcept {
      return ptr_.owner_before(other);
    }

    void swap(StrictSharedPtr &other) noexcept {
      ptr_.swap(other.ptr_);
    }

    friend void swap(StrictSharedPtr &a, StrictSharedPtr &b) noexcept {
      a.swap(b);
    }

    explicit operator bool() const noexcept {
      return static_cast<bool>(ptr_);
    }

    bool operator==(const StrictSharedPtr &other) const noexcept = default;
    bool operator!=(const StrictSharedPtr &other) const noexcept = default;

    bool operator==(const std::shared_ptr<T> &other) const noexcept {
      return ptr_ == other;
    }

    bool operator!=(const std::shared_ptr<T> &other) const noexcept {
      return ptr_ != other;
    }

    bool operator<(const StrictSharedPtr &other) const noexcept {
      return ptr_.owner_before(other.ptr_);
    }

    template <typename Index>
      requires requires(const T &t, Index i) {
        { t[i] };  // enables operator[] only if T supports indexing
      }
    decltype(auto) operator[](Index i) const {
      return (*ptr_)[i];
    }

   private:
    std::shared_ptr<T> ptr_;
  };

}  // namespace qtils
