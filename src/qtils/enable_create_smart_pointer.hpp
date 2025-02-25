/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Custom memory management utilities for creating shared and unique
 * pointers using malloc and free.
 *
 * This module provides custom allocators and deleters for managing dynamic
 * memory using standard malloc and free while maintaining compatibility with
 * C++ smart pointers.
 *
 * Purpose:
 * - Enable the creation of smart pointers for objects with hidden
 * constructors/destructors, allowing controlled instantiation and destruction
 * of such objects.
 *
 * Features:
 * - @class create_shared_allocator: Custom allocator for std::shared_ptr
 * - @class create_unique_deleter: Custom deleter for std::unique_ptr
 * - @class enable_create_shared: Enables creation of shared pointers with
 * custom allocation
 * - @class enable_create_unique: Enables creation of unique pointers with
 * custom deallocation
 *
 * These utilities allow fine-grained control over memory management,
 * making them suitable for performance-critical applications.
 */

#pragma once

#include <memory>

namespace qtils {

  namespace detail {

    /**
     * @brief Custom allocator for creating shared pointers using std::malloc
     * and std::free.
     *
     * @tparam T Type of object to allocate.
     */
    template <typename T>
    struct create_shared_allocator {
      using value_type = T;

      create_shared_allocator() = default;

      /**
       * @brief Copy constructor for allocator of a different type.
       *
       * @tparam U Type of the other allocator.
       * @param other Allocator to copy from.
       */
      template <typename U>
      explicit create_shared_allocator(const create_shared_allocator<U> &) {}

      /**
       * @brief Rebinds the allocator to a different type.
       *
       * @tparam U The new type.
       */
      template <typename U>
      struct rebind {
        using other = create_shared_allocator<U>;
      };

      /**
       * @brief Allocates memory for n objects of type T.
       *
       * @param n Number of objects to allocate.
       * @return Pointer to allocated memory.
       */
      T *allocate(std::size_t n) {
        return static_cast<T *>(std::malloc(n * sizeof(T)));
      }

      /**
       * @brief Deallocates memory for the given pointer.
       *
       * @param p Pointer to memory to deallocate.
       */
      void deallocate(T *p, std::size_t) {
        std::free(p);
      }
    };

    /**
     * @brief Custom deleter for unique pointers using std::free.
     *
     * @tparam T Type of object to delete.
     */
    template <typename T>
    struct create_unique_deleter {
      /**
       * @brief Calls the destructor and frees the allocated memory.
       *
       * @param ptr Pointer to the object to delete.
       */
      void operator()(T *ptr) const noexcept {
        ptr->~T();
        std::free(ptr);
      }
    };

  }  // namespace detail

  /**
   * @brief Enables creation of shared pointers using a custom allocator.
   *
   * @tparam T Type of the object to create as a shared pointer.
   */
  template <typename T>
  struct enable_create_shared : std::enable_shared_from_this<T> {
    /**
     * @brief Creates a shared pointer with custom allocation.
     *
     * @tparam Args Constructor arguments for T.
     * @param args Arguments to forward to T's constructor.
     * @return std::shared_ptr<T> Shared pointer to the created object.
     */
    template <typename... Args>
    static std::shared_ptr<T> create_shared(Args &&...args) {
      return std::allocate_shared<T>(
          detail::create_shared_allocator<T>(), std::forward<Args>(args)...);
    }
  };

  /**
   * @brief Enables creation of unique pointers with custom deallocation.
   *
   * @tparam T Type of the object to create as a unique pointer.
   */
  template <typename T>
  struct enable_create_unique {
    /**
     * @brief Creates a unique pointer with custom allocation and deletion.
     *
     * @tparam Args Constructor arguments for T.
     * @param args Arguments to forward to T's constructor.
     * @return std::unique_ptr<T, detail::create_unique_deleter<T>> Unique
     * pointer to the created object.
     */
    template <typename... Args>
    static std::unique_ptr<T, detail::create_unique_deleter<T>> create_unique(
        Args &&...args) {
      T *raw_ptr = static_cast<T *>(std::malloc(sizeof(T)));
      [[unlikely]] if (not raw_ptr) { throw std::bad_alloc(); }
      try {
        new (raw_ptr) T(std::forward<Args>(args)...);
      } catch (...) {
        std::free(raw_ptr);
        throw;
      }
      return std::unique_ptr<T, detail::create_unique_deleter<T>>(raw_ptr);
    }
  };

}  // namespace qtils
