/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @brief This header provides macros to create factory methods for smart
 * pointers (shared_ptr and unique_ptr) that allow access to private
 * constructors and destructors of a class.
 *
 * These macros enable the creation of shared and unique smart pointers even
 * when the constructors or destructors of a class are private. This is achieved
 * by utilizing a helper struct (NoConstruct) that manages the memory and
 * lifetime of the object while allowing access to private members through
 * placement new.
 *
 * The macros are:
 * - CREATE_SHARED_METHOD(Self): Factory method for shared_ptr
 * - CREATE_UNIQUE_METHOD(Self): Factory method for unique_ptr
 *
 * @note These macros ensure proper destruction of the object by explicitly
 * calling the destructor in NoConstruct's destructor.
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
 */
#define CREATE_SHARED_METHOD(Self)                                    \
  template <typename... Args>                                         \
  static decltype(auto) create_shared(Args &&...args) {               \
    struct NoConstruct : std::enable_shared_from_this<NoConstruct> {  \
      unsigned char mem[sizeof(Self)]; /* <- must be first  member */ \
      bool constructed = false;        /* <- must be last member   */ \
      ~NoConstruct() {                                                \
        if (constructed) ((Self *)mem)->~Self();                      \
      }                                                               \
    };                                                                \
    auto tmp = std::make_shared<NoConstruct>();                       \
    auto ptr = new (tmp->mem) Self(std::forward<Args>(args)...);      \
    tmp->constructed = true;                                          \
    return std::shared_ptr<Self>(tmp, ptr);                           \
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
#define CREATE_UNIQUE_METHOD(Self)                                    \
  template <typename... Args>                                         \
  static decltype(auto) create_unique(Args &&...args) {               \
    struct NoConstruct {                                              \
      unsigned char mem[sizeof(Self)]; /* <- must be first  member */ \
      bool constructed = false;        /* <- must be last member   */ \
      ~NoConstruct() {                                                \
        if (constructed) ((Self *)mem)->~Self();                      \
      }                                                               \
    };                                                                \
    auto tmp = new NoConstruct();                                     \
    auto ptr = new (tmp->mem) Self(std::forward<Args>(args)...);      \
    tmp->constructed = true;                                          \
    return std::unique_ptr<Self, std::function<void(Self *)>>(        \
        ptr, [tmp](Self *obj) mutable { delete tmp; });               \
  }
