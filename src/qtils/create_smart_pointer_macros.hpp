/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <memory>

/**
 * @brief This header provides macros to create factory methods for smart
 * pointers (shared_ptr and unique_ptr) that allow access to private
 * constructors and destructors of a class.
 *
 * These macros enable the creation of shared and unique smart pointers even
 * when the constructors or destructors of a class are private.
 *
 * The macros are:
 * - CREATE_SHARED_METHOD(Self): Factory method for shared_ptr
 * - CREATE_UNIQUE_METHOD(Self): Factory method for unique_ptr
 */

/**
 * @brief Factory method for creating a std::shared_ptr with access to private
 * members.
 *
 * This macro generates a static method `create_shared` that returns a
 * std::shared_ptr to an instance of the class, even if the constructor is
 * private. Memory management is handled through a helper struct `NoConstruct`,
 * ensuring proper destruction of the object.
 *
 * @param Self The class type for which the shared_ptr factory method is
 * created.
 * @note Self should not inherit std::enable_shared_from_this<Self> because
 * CREATE_SHARED_METHOD(Self) already provides the same methods.
 *
 * @details
 * The main idea is to use std::make_shared to allocate memory atomically
 * for both the control block and the managed object. This is achieved
 * through the helper struct NoConstruct, which inherits from
 * std::enable_shared_from_this.
 *
 * - @b NoConstruct:
 *   - Inherits std::enable_shared_from_this to ensure that memory for the
 *     control block and the managed object is allocated in a single
 *     system call.
 *   - Serves as a way to delay the constructor call for the target object.
 *   - Contains a byte array `mem` large enough to hold an instance of Self.
 *     The constructor of Self is explicitly called using placement new
 *     in the `create_shared` method, allowing access even to private
 *     constructors and destructors.
 *   - The `constructed` flag indicates whether the constructor has been
 *     called. The destructor of NoConstruct checks this flag to explicitly
 *     call the destructor of Self if necessary.
 *
 * - @b placement_new:
 *   - Used to explicitly call the constructor in the already allocated
 *     memory (`mem`).
 *   - Allows creation of an instance even if the constructor is private.
 *
 * - @b weak_this_:
 *   - Stores a std::weak_ptr to the instance of Self.
 *   - Used in weak_from_this and shared_from_this methods to return
 *     weak or shared pointers to the current object.
 *
 * - @b enable_shared_from_this:
 *   - A SFINAE check with std::declval ensures that Self does not
 *     directly inherit from std::enable_shared_from_this, since
 *     CREATE_SHARED_METHOD already provides the equivalent methods.
 */
#define CREATE_SHARED_METHOD(Self)                                           \
 private:                                                                    \
  struct derived_from {                                                      \
    template <typename T>                                                    \
    static auto test(int)                                                    \
        -> decltype(std::declval<T>().shared_from_this(), std::true_type{}); \
    template <typename>                                                      \
    static std::false_type test(...);                                        \
    static constexpr bool enable_shared_from_this =                          \
        decltype(test<Self>(0))::value;                                      \
  };                                                                         \
  static_assert(not derived_from::enable_shared_from_this,                   \
      "Don't inherit std::enable_shared_from_this<" #Self "> as "            \
      "CREATE_SHARED_METHOD(" #Self ") already adds the same methods");      \
  mutable std::weak_ptr<Self> weak_this_;                                    \
                                                                             \
 public:                                                                     \
  template <typename... Args>                                                \
  static decltype(auto) create_shared(Args &&...args) {                      \
    struct NoConstruct : std::enable_shared_from_this<NoConstruct> {         \
      unsigned char mem[sizeof(Self)]; /* <- must be first member */         \
      bool constructed = false;        /* <- must be last member  */         \
      ~NoConstruct() { if (constructed) ((Self *)mem)->~Self(); }            \
    };                                                                       \
    auto tmp = std::make_shared<NoConstruct>();                              \
    auto ptr = new (tmp->mem) Self(std::forward<Args>(args)...);             \
    tmp->constructed = true;                                                 \
    ptr->weak_this_ = std::shared_ptr<Self>(tmp, ptr);                       \
    return ptr->shared_from_this();                                          \
  }                                                                          \
  std::weak_ptr<const Self> weak_from_this() const {                         \
    return weak_this_;                                                       \
  }                                                                          \
  std::weak_ptr<Self> weak_from_this() {                                     \
    return weak_this_;                                                       \
  }                                                                          \
  std::shared_ptr<const Self> shared_from_this() const {                     \
    [[likely]] if (auto sptr = weak_this_.lock()) { return sptr; }           \
    throw std::bad_weak_ptr();                                               \
  }                                                                          \
  std::shared_ptr<Self> shared_from_this() {                                 \
    [[likely]] if (auto sptr = weak_this_.lock()) { return sptr; }           \
    throw std::bad_weak_ptr();                                               \
  }

/**
 * @brief Factory method for creating a std::unique_ptr with access to private
 * members.
 *
 * This macro generates a static method `create_unique` that returns a
 * std::unique_ptr to an instance of the class, even if the constructor is
 * private. Memory management is handled through a helper struct
 * `NoConstruct`, ensuring proper destruction of the object.
 *
 * @param Self The class type for which the unique_ptr factory method is
 * created.
 */
#define CREATE_UNIQUE_METHOD(Self)                                   \
  template <typename... Args>                                        \
  static decltype(auto) create_unique(Args &&...args) {              \
    struct NoConstruct {                                             \
      unsigned char mem[sizeof(Self)]; /* <- must be first member */ \
      bool constructed = false;        /* <- must be last member  */ \
      ~NoConstruct() { if (constructed) ((Self *)mem)->~Self(); }    \
    };                                                               \
    auto tmp = new NoConstruct();                                    \
    try {                                                            \
      auto ptr = new (tmp->mem) Self(std::forward<Args>(args)...);   \
      tmp->constructed = true;                                       \
      return std::unique_ptr<Self, std::function<void(Self *)>>(     \
          ptr, [tmp](Self *obj) mutable { delete tmp; });            \
    } catch (...) {                                                  \
      delete tmp;                                                    \
      throw;                                                         \
    }                                                                \
  }
