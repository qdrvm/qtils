/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <string_view>
#include <type_traits>

namespace qtils {

  namespace detail {
    template <typename T>
    concept NeedsWrapper = not std::is_class_v<T> and not std::is_array_v<T>
        and not std::is_void_v<T>;

    template <typename T>
    struct Wrapper {
      Wrapper() = default;

      template <typename... Args>
        requires std::is_constructible_v<T, Args...>
      explicit Wrapper(Args &&...args) : value(std::forward<Args>(args)...) {}

     protected:
      T value;
    };

    template <std::size_t N>
    struct StringLiteral {
      constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
      }
      constexpr std::string_view view() const {
        return std::string_view(value, N - 1);  // remove '\0'
      }
      char value[N];
    };

    template <typename From, typename To>
    struct copy_qualifiers {
     private:
      using T0 = std::remove_cvref_t<To>;

      using T1 = std::conditional_t<  //
          std::is_lvalue_reference_v<From>,
          std::add_lvalue_reference_t<T0>,
          T0>;

      using T2 = std::conditional_t<  //
          std::is_rvalue_reference_v<From>,
          std::add_rvalue_reference_t<T1>,
          T1>;

      using T3 = std::conditional_t<  //
          std::is_const_v<From>,
          std::add_const_t<T2>,
          T2>;

      using T4 = std::conditional_t<  //
          std::is_volatile_v<From>,
          std::add_volatile_t<T3>,
          T3>;

     public:
      using type = T4;
    };

    template <typename From, typename To>
    using copy_qualifiers_t = typename copy_qualifiers<From, To>::type;
  }  // namespace detail

  template <typename T, typename Tag_>
  class Tagged;

  template <typename X>
  struct IsTaggedHelper : std::false_type {};

  template <typename X, typename Y>
  struct IsTaggedHelper<Tagged<X, Y>> : std::true_type {};

  template <typename X>
  concept IsTagged = IsTaggedHelper<std::remove_cvref_t<X>>::value;

  template <typename T, typename Enable = void>
  struct OrigTypeHelper {
    using Type = std::remove_cvref_t<T>;
  };

  template <typename T>
  struct OrigTypeHelper<T, std::enable_if_t<IsTagged<T>>> {
    using Type = typename std::remove_cvref_t<T>::Type;
  };

  template <typename T>
  using OrigType = typename OrigTypeHelper<T>::Type;

  template <typename T, typename Tag_>
  class Tagged : public std::conditional_t<detail::NeedsWrapper<T>,
                     detail::Wrapper<T>,
                     T> {
   public:
    static constexpr bool IsWrapped = detail::NeedsWrapper<T>;
    using Base = std::conditional_t<IsWrapped, detail::Wrapper<T>, T>;
    using Type = T;
    using Tag = Tag_;

    Tagged() = default;

    template <typename... Args>
      requires std::is_constructible_v<Base, Args...>
    explicit Tagged(Args &&...args) : Base(std::forward<Args>(args)...) {}

    // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
    Tagged &operator=(T &&value) noexcept(
        std::is_nothrow_assignable_v<Base &, T>) {
      if constexpr (IsWrapped) {
        this->value = std::forward<T>(value);
      } else if constexpr (std::is_assignable_v<Base &, T>) {
        Base::operator=(std::forward<T>(value));
      }
      return *this;
    }

    template <typename Out>
      requires std::is_constructible_v<Out, T>
    explicit operator Out() const {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return this->Base::value;
      } else {
        return *this;
      }
    }

   private:
    // Makes tagged result if R based on original type
    template <typename R, typename E = T>
    using Result = std::conditional_t<not std::is_same_v<E, T>,
        E,
        std::conditional_t<std::is_same_v<std::remove_cvref_t<R>, T>,
            detail::copy_qualifiers_t<R, Tagged>,
            R>>;

   public:
    // Support all available operators of original type

#define OPA(OP)                                                   \
  template <typename R>                                           \
    requires requires {                                           \
      std::declval<T &>() OP std::declval<const OrigType<R> &>(); \
    }                                                             \
  decltype(auto) operator OP(const R &r) {                        \
    if constexpr (IsWrapped) {                                    \
      this->value OP static_cast<const OrigType<R> &>(r);         \
    } else if constexpr (std::is_assignable_v<Base &, T>) {       \
      Base::operator OP(static_cast<const OrigType<R> &>(r));     \
    }                                                             \
  }                                                               \
                                                                  \
  template <typename L>                                           \
    requires(not IsTagged<L>) and requires {                      \
      std::forward<L>(std::declval<std::remove_cvref_t<L>>()) OP  \
      std::declval<const T &>();                                  \
    }                                                             \
  friend decltype(auto) operator OP(L &&l, const Tagged &r) {     \
    return std::forward<L>(l) OP static_cast<const T &>(r);       \
  }

#define OP1(OP)                                         \
  decltype(auto) operator OP() const                    \
    requires requires { OP std::declval<const T &>(); } \
  {                                                     \
    return OP static_cast<const T &>(*this);            \
  }

#define OP2(OP)                                                         \
  template <typename U>                                                 \
    requires requires {                                                 \
      std::declval<const T &>() OP std::declval<const OrigType<U> &>(); \
    }                                                                   \
  decltype(auto) operator OP(const U &r) const {                        \
    return static_cast<const T &>(*this)                                \
        OP static_cast<const OrigType<U> &>(r);                         \
  }                                                                     \
                                                                        \
  template <typename L>                                                 \
    requires(not IsTagged<L>) and requires {                            \
      std::forward<L>(std::declval<std::remove_cvref_t<L>>()) OP        \
      std::declval<const T &>();                                        \
    }                                                                   \
  friend decltype(auto) operator OP(L &&l, const Tagged &r) {           \
    return std::forward<L>(l) OP static_cast<const T &>(r);             \
  }

    OPA(+=)
    OPA(-=)
    OPA(*=)
    OPA(/=)
    OPA(%=)
    OPA(&=)
    OPA(|=)
    OPA(^=)
    OPA(<<=)
    OPA(>>=)

    OP2(==)
    OP2(!=)
    OP2(+)
    OP2(-)
    OP2(*)
    OP2(/)
    OP2(%)
    OP2(&)
    OP2(|)
    OP2(^)
    OP2(>)
    OP2(<)
    OP2(>=)
    OP2(<=)
    OP2(&&)
    OP2(||)
    OP2(<<)
    OP2(>>)

    OP1(~)
    OP1(!)

#undef OP1
#undef OP3
#undef OPA

    decltype(auto) operator++()
      requires requires(T &t) { ++t; }
    {
      if constexpr (IsWrapped) {
        ++this->value;
      } else {
        Base::operator++();
      }
      return *this;
    }

    decltype(auto) operator--()
      requires requires(T &t) { --t; }
    {
      if constexpr (IsWrapped) {
        --this->value;
      } else {
        Base::operator--();
      }
      return *this;
    }

    decltype(auto) operator++(int)
      requires requires(T &t) { t++; }
    {
      auto tmp = *this;
      if constexpr (IsWrapped) {
        ++this->value;
      } else {
        Base::operator++();
      }
      return tmp;
    }

    decltype(auto) operator--(int)
      requires requires(T &t) { t--; }
    {
      auto tmp = *this;
      if constexpr (IsWrapped) {
        --this->value;
      } else {
        Base::operator--();
      }
      return tmp;
    }
  };

  /// Number-based marker-type for using as tag
  template <size_t Num>
  struct NumTag {
    static constexpr size_t tag = Num;
  };

  /// String-based marker-type for using as tag
  template <detail::StringLiteral Str>
  struct StrTag {
    static constexpr std::string_view tag = Str.view();
  };

}  // namespace qtils
