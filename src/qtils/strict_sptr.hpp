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
   * Prevents null ownership and disables unsafe operations such as reset()
   * and assignment from raw or shared_ptr. Supports optional custom deleters.
   *
   * @tparam T The type of the managed object.
   * @tparam Deleter The type of deleter used to destroy the object.
   */
  template <class T, class Deleter = std::default_delete<T>>
    requires(not std::is_void_v<T>)
  class StrictSharedPtr {
   public:
    /// @brief Default constructor is disabled
    StrictSharedPtr() = delete;

    /// @brief Nullptr constructor is disabled
    StrictSharedPtr(std::nullptr_t) = delete;

    /**
     * @brief Constructs from an existing std::shared_ptr<T>
     * @param sptr The shared pointer to wrap
     * @throws std::invalid_argument if sptr is null
     */
    // NOLINTNEXTLINE(google-explicit-constructor)
    StrictSharedPtr(std::shared_ptr<T> sptr) : ptr_(std::move(sptr)) {
      if (!ptr_) {
        throw std::invalid_argument("Shared pointer is null");
      }
    }

    /**
     * @brief Constructs from raw pointer and custom deleter
     * @param raw Raw pointer to managed object
     * @param deleter Deleter to be used
     * @throws std::invalid_argument if raw is null
     */
    StrictSharedPtr(T *raw, Deleter deleter)
        : ptr_(std::shared_ptr<T>(raw, std::move(deleter))) {
      if (!ptr_) {
        throw std::invalid_argument("Shared pointer is null");
      }
    }

  private:
    /**
     * @brief Special tag type to select the DI constructor overload.
     *
     * This struct is used to differentiate the constructor intended
     * specifically for Dependency Injection (DI) frameworks.
     */
    struct ForceInject {};

  public:
    /**
     * @brief Constructor for use by Dependency Injection (DI) frameworks (e.g., Boost.DI).
     *
     * DI frameworks typically prefer constructors with the maximum number
     * of parameters when resolving dependencies. This overload includes
     * an extra dummy tag (`ForceInject`) and a variadic parameter pack (`...`) to
     * ensure it has the highest arity and is therefore selected by the injector.
     *
     * Unlike the standard constructor, this version **bypasses the null-check**.
     * It assumes the DI container guarantees that the passed `shared_ptr`
     * is valid and correctly constructed.
     *
     * The constructor is not intended for general use. It can only be called
     * explicitly with the `ForceInject` tag to avoid accidental misuse.
     *
     * @param sptr The shared_ptr to wrap.
     * @param Dummy The ForceInject tag to select this constructor.
     * @param ... Additional dummy parameters to increase constructor arity.
     */
    StrictSharedPtr(std::shared_ptr<T> sptr, ForceInject, ...)
        : StrictSharedPtr(std::move(sptr)) {}

    /// @brief Copy constructor
    StrictSharedPtr(const StrictSharedPtr &) = default;

    /// @brief Copy assignment
    StrictSharedPtr &operator=(const StrictSharedPtr &) = default;

    /// @brief Move constructor
    StrictSharedPtr(StrictSharedPtr &&) noexcept = default;

    /// @brief Move assignment
    StrictSharedPtr &operator=(StrictSharedPtr &&) noexcept = default;

    /// @brief Access underlying pointer
    T *operator->() const noexcept {
      return ptr_.get();
    }

    /// @brief Dereference the pointer
    T &operator*() const noexcept {
      return *ptr_;
    }

    /**
     * @brief Implicit conversion to std::shared_ptr<T>
     * @return A copy of the managed shared_ptr
     */
    // NOLINTNEXTLINE(google-explicit-constructor)
    operator std::shared_ptr<T>() const noexcept {
      return ptr_;
    }

    /// @brief Get raw pointer
    T *get() const noexcept {
      return ptr_.get();
    }

    /// @brief Get number of shared owners
    size_t use_count() const noexcept {
      return ptr_.use_count();
    }

    /**
     * @brief Compare ownership with another StrictSharedPtr
     * @tparam U Other type
     * @param other Another StrictSharedPtr
     * @return true if this precedes other in ownership
     */
    template <typename U>
    bool owner_before(const StrictSharedPtr<U> &other) const noexcept {
      return ptr_.owner_before(other.ptr_);
    }

    /**
     * @brief Compare ownership with a raw std::shared_ptr
     * @tparam U Other type
     * @param other Shared pointer
     * @return true if this precedes other in ownership
     */
    template <typename U>
    bool owner_before(const std::shared_ptr<U> &other) const noexcept {
      return ptr_.owner_before(other);
    }

    /// @brief Swap with another StrictSharedPtr
    void swap(StrictSharedPtr &other) noexcept {
      ptr_.swap(other.ptr_);
    }

    /// @brief ADL-friendly swap overload
    friend void swap(StrictSharedPtr &a, StrictSharedPtr &b) noexcept {
      a.swap(b);
    }

    /// @brief Check if pointer is not null
    explicit operator bool() const noexcept {
      return static_cast<bool>(ptr_);
    }

    /// @brief Equality comparison
    bool operator==(const StrictSharedPtr &other) const noexcept = default;

    /// @brief Inequality comparison
    bool operator!=(const StrictSharedPtr &other) const noexcept = default;

    /// @brief Compare with raw shared_ptr
    bool operator==(const std::shared_ptr<T> &other) const noexcept {
      return ptr_ == other;
    }

    /// @brief Compare with raw shared_ptr
    bool operator!=(const std::shared_ptr<T> &other) const noexcept {
      return ptr_ != other;
    }

    /// @brief Less-than comparison for ordered containers
    bool operator<(const StrictSharedPtr &other) const noexcept {
      return ptr_.owner_before(other.ptr_);
    }

    /**
     * @brief Indexing operator (only if T supports indexing)
     * @tparam Index Index type
     * @param i Index value
     * @return Result of indexing T
     */
    template <typename Index>
      requires requires(const T &t, Index i) {
        { t[i] };
      }
    decltype(auto) operator[](Index i) const {
      return (*ptr_)[i];
    }

    /**
     * @brief Access the stored deleter (if known at compile time)
     * @return Pointer to deleter object, or nullptr if not found
     */
    Deleter *get_deleter() const noexcept {
      return std::get_deleter<Deleter>(ptr_);
    }

   private:
    std::shared_ptr<T> ptr_;
  };

}  // namespace qtils