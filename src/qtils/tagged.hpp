/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <string_view>
#include <type_traits>

// Macro to define a type trait that checks

#define DEFINE_UNARY_OPERATOR_CHECK(op_name, op)                          \
  template <typename ARG, typename = void>                                \
  struct is_##op_name : std::false_type {};                               \
                                                                          \
  template <typename ARG>                                                 \
  struct is_##op_name<ARG, std::void_t<decltype(op std::declval<ARG>())>> \
      : std::integral_constant<bool, IsTagged<std::remove_cvref_t<ARG>>> {};

#define DEFINE_BINARY_OPERATOR_CHECK(op_name, op)                              \
  template <typename LHS, typename RHS, typename = void>                       \
  struct is_##op_name : std::false_type {};                                    \
                                                                               \
  template <typename LHS, typename RHS>                                        \
  struct is_##op_name<                                                         \
      LHS,                                                                     \
      RHS,                                                                     \
      std::void_t<decltype(std::declval<LHS>() op std::declval<RHS>())>>       \
      : std::integral_constant<bool, not IsTagged<std::remove_cvref_t<LHS>>> { \
  };

#define DEFINE_COMPOUND_OPERATOR_CHECK(op_name, op)                            \
  template <typename LHS, typename RHS, typename = void>                       \
  struct is_##op_name : std::false_type {};                                    \
                                                                               \
  template <typename LHS, typename RHS>                                        \
  struct is_##op_name<                                                         \
      LHS,                                                                     \
      RHS,                                                                     \
      std::void_t<decltype(std::declval<LHS &>() op std::declval<RHS>())>>     \
      : std::integral_constant<bool, not IsTagged<std::remove_cvref_t<LHS>>> { \
  };

// Macro to define a friend operator inside the class
#define DEFINE_UNARY_OPERATOR(op_name, op)                          \
  decltype(auto) operator op() const &                              \
    requires requires { std::declval<const T &>().operator op(); }  \
  {                                                                 \
    return _get_T().operator op();                                  \
  }                                                                 \
  decltype(auto) operator op() &                                    \
    requires requires { op std::declval<T &>().operator op(); }     \
  {                                                                 \
    return _get_T().operator op();                                  \
  }                                                                 \
  decltype(auto) operator op() const &&                             \
    requires requires { std::declval<const T &&>().operator op(); } \
  {                                                                 \
    return _copy_T().operator op();                                 \
  }                                                                 \
  decltype(auto) operator op() &&                                   \
    requires requires { std::declval<T &&>().operator op(); }       \
  {                                                                 \
    return _move_T().operator op();                                 \
  }                                                                 \
                                                                    \
  template <typename ARG>                                           \
    requires(std::is_same_v<std::remove_cvref_t<ARG>, Tagged>)      \
        and requires { op std::declval<const T &>(); }              \
  friend constexpr decltype(auto) operator op(const ARG &a) {       \
    return op(a._get_T());                                          \
  }                                                                 \
  template <typename ARG>                                           \
    requires(std::is_same_v<std::remove_cvref_t<ARG>, Tagged>)      \
        and requires { op std::declval<T &>(); }                    \
  friend constexpr decltype(auto) operator op(ARG &a) {             \
    return op(a._get_T());                                          \
  }                                                                 \
  template <typename ARG>                                           \
    requires(std::is_same_v<std::remove_cvref_t<ARG>, Tagged>)      \
        and requires { op std::declval<const T &&>(); }             \
  friend constexpr decltype(auto) operator op(const ARG &&a) {      \
    return op(a._copy_T());                                         \
  }                                                                 \
  template <typename ARG>                                           \
    requires(std::is_same_v<std::remove_cvref_t<ARG>, Tagged>)      \
        and requires { op std::declval<T &&>(); }                   \
  friend constexpr decltype(auto) operator op(ARG &&a) {            \
    return op(a._move_T());                                         \
  }

#define DEFINE_SUFFIX_OPERATOR(op_name, op)                          \
  decltype(auto) operator op(int) const &                            \
    requires requires { std::declval<const T &>().operator op(0); }  \
  {                                                                  \
    return _get_T().operator op(0);                                  \
  }                                                                  \
  decltype(auto) operator op(int) &                                  \
    requires requires { op std::declval<T &>().operator op(0); }     \
  {                                                                  \
    return _get_T().operator op(0);                                  \
  }                                                                  \
  decltype(auto) operator op(int) const &&                           \
    requires requires { std::declval<const T &&>().operator op(0); } \
  {                                                                  \
    return _copy_T().operator op(0);                                 \
  }                                                                  \
  decltype(auto) operator op(int) &&                                 \
    requires requires { std::declval<T &&>().operator op(0); }       \
  {                                                                  \
    return _move_T().operator op(0);                                 \
  }                                                                  \
                                                                     \
  template <typename ARG>                                            \
    requires(std::is_same_v<std::remove_cvref_t<ARG>, Tagged>)       \
        and requires { std::declval<const T &>() op; }               \
  friend constexpr decltype(auto) operator op(const ARG &a, int) {   \
    return (a._get_T())op;                                           \
  }                                                                  \
  template <typename ARG>                                            \
    requires(std::is_same_v<std::remove_cvref_t<ARG>, Tagged>)       \
        and requires { std::declval<T &>() op; }                     \
  friend constexpr decltype(auto) operator op(ARG &a, int) {         \
    return (a._get_T())op;                                           \
  }                                                                  \
  template <typename ARG>                                            \
    requires(std::is_same_v<std::remove_cvref_t<ARG>, Tagged>)       \
        and requires { std::declval<const T &&>() op; }              \
  friend constexpr decltype(auto) operator op(const ARG &&a, int) {  \
    return (a._copy_T())op;                                          \
  }                                                                  \
  template <typename ARG>                                            \
    requires(std::is_same_v<std::remove_cvref_t<ARG>, Tagged>)       \
        and requires { std::declval<T &&>() op; }                    \
  friend constexpr decltype(auto) operator op(ARG &&a, int) {        \
    return (a._move_T())op;                                          \
  }

// Macro to define a friend operator inside the class
#define DEFINE_BINARY_OPERATOR(op_name, op)                                    \
  template <typename LHS>                                                      \
    requires(not std::is_same_v<std::remove_cvref_t<LHS>, Tagged>)             \
        and requires { std::declval<LHS &&>() op std::declval<const T &>(); }  \
  friend constexpr auto operator op(LHS &&lhs, const Tagged &rhs) {            \
    return std::forward<LHS>(lhs) op rhs._get_T();                             \
  }                                                                            \
                                                                               \
  template <typename LHS>                                                      \
    requires(not std::is_same_v<std::remove_cvref_t<LHS>, Tagged>)             \
        and requires { std::declval<LHS &&>() op std::declval<T &>(); }        \
  friend constexpr auto operator op(LHS &&lhs, Tagged &rhs) {                  \
    return std::forward<LHS>(lhs) op rhs._get_T();                             \
  };                                                                           \
                                                                               \
  template <typename LHS>                                                      \
    requires(not std::is_same_v<std::remove_cvref_t<LHS>, Tagged>)             \
        and requires { std::declval<LHS &&>() op std::declval<const T &&>(); } \
  friend constexpr auto operator op(LHS &&lhs, const Tagged &&rhs) {           \
    return std::forward<LHS>(lhs) op rhs._copy_T();                            \
  }                                                                            \
                                                                               \
  template <typename LHS>                                                      \
    requires(not std::is_same_v<std::remove_cvref_t<LHS>, Tagged>)             \
        and requires { std::declval<LHS &&>() op std::declval<T &&>(); }       \
  friend constexpr auto operator op(LHS &&lhs, Tagged &&rhs) {                 \
    return std::forward<LHS>(lhs) op rhs._move_T();                            \
  }

#define DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(op_name, op)                       \
  template <typename R>                                                        \
    requires requires {                                                        \
      std::declval<T &>() op                                                   \
      std::declval<detail::copy_qualifiers_t<R, OrigType<R>>>();               \
    }                                                                          \
  decltype(auto) operator op(R &&r) {                                          \
    auto &&lhs_val = [&](auto &&arg) -> decltype(auto) {                       \
      if constexpr (IsTagged<std::remove_cvref_t<decltype(arg)>>) {            \
        return static_cast<T>(arg);                                            \
      } else {                                                                 \
        return std::forward<decltype(arg)>(arg);                               \
      }                                                                        \
    }(*this);                                                                  \
    auto &&rhs_val = [&](auto &&arg) -> decltype(auto) {                       \
      if constexpr (IsTagged<std::remove_cvref_t<decltype(arg)>>) {            \
        return static_cast<OrigType<std::remove_cvref_t<decltype(arg)>>>(arg); \
      } else {                                                                 \
        return std::forward<decltype(arg)>(arg);                               \
      }                                                                        \
    }(std::forward<R>(r));                                                     \
    if constexpr (requires { lhs_val op rhs_val; }) {                          \
      *this = Tagged<T, Tag>(lhs_val op rhs_val);                              \
    } else {                                                                   \
      static_assert([] { return false; }(),                                    \
                    "Compound assignment operator " #op                        \
                    " is not available for base type");                        \
    }                                                                          \
    return *this;                                                              \
  }                                                                            \
  DEFINE_BINARY_OPERATOR(op_name, op)


namespace qtils {

  template <typename T, typename Tag>
  class Tagged;

  template <typename T>
  struct IsTaggedHelper : std::false_type {};

  template <typename T, typename Tag>
  struct IsTaggedHelper<Tagged<T, Tag>> : std::true_type {};

  template <typename X>
  concept IsTagged = IsTaggedHelper<std::remove_cvref_t<X>>::value;

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

    //    DEFINE_UNARY_OPERATOR_CHECK(bitwise_not, ~)
    //    DEFINE_UNARY_OPERATOR_CHECK(locical_not, !)
    //    DEFINE_UNARY_OPERATOR_CHECK(pre_increment, ++)
    //    DEFINE_UNARY_OPERATOR_CHECK(pre_decrement, --)
    //    //    DEFINE_SUFFIX_OPERATOR_CHECK(post_increment, ++)
    //    //    DEFINE_SUFFIX_OPERATOR_CHECK(post_decrement, --)

    // Define type traits for all required operators
//    DEFINE_BINARY_OPERATOR_CHECK(shift_left, <<)
//    DEFINE_BINARY_OPERATOR_CHECK(shift_right, >>)
//    DEFINE_BINARY_OPERATOR_CHECK(add, +)
//    DEFINE_BINARY_OPERATOR_CHECK(subtract, -)
//    DEFINE_BINARY_OPERATOR_CHECK(multiply, *)
//    DEFINE_BINARY_OPERATOR_CHECK(divide, /)
//    DEFINE_BINARY_OPERATOR_CHECK(modulus, %)
//    DEFINE_BINARY_OPERATOR_CHECK(bitwise_and, &)
//    DEFINE_BINARY_OPERATOR_CHECK(bitwise_or, |)
//    DEFINE_BINARY_OPERATOR_CHECK(bitwise_xor, ^)
//    DEFINE_BINARY_OPERATOR_CHECK(logical_and, &&)
//    DEFINE_BINARY_OPERATOR_CHECK(logical_or, ||)
//    DEFINE_BINARY_OPERATOR_CHECK(equal, ==)
//    DEFINE_BINARY_OPERATOR_CHECK(not_equal, !=)
//    DEFINE_BINARY_OPERATOR_CHECK(less, <)
//    DEFINE_BINARY_OPERATOR_CHECK(less_equal, <=)
//    DEFINE_BINARY_OPERATOR_CHECK(greater, >)
//    DEFINE_BINARY_OPERATOR_CHECK(greater_equal, >=)
//
//    DEFINE_COMPOUND_OPERATOR_CHECK(plus_assign, +=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(minus_assign, -=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(multiply_assign, *=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(divide_assign, /=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(modulus_assign, %=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(bitwise_and_assign, &=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(bitwise_or_assign, |=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(bitwise_xor_assign, ^=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(shift_left_assign, <<=)
//    DEFINE_COMPOUND_OPERATOR_CHECK(shift_right_assign, >>=)

  }  // namespace detail

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

  template <typename LHS, typename T, typename = void>
  struct is_streamable : std::false_type {};

  template <typename LHS, typename T>
  struct is_streamable<
      LHS,
      T,
      std::void_t<decltype(std::declval<LHS &>() << std::declval<T>())>>
      : std::true_type {};


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

    Tagged &operator=(auto &&value) noexcept(
        std::is_nothrow_assignable_v<Base &, decltype(value)>) {
      if constexpr (IsWrapped) {
        this->value = std::forward<T>(value);
      } else if constexpr (std::is_assignable_v<Base &, decltype(value)>) {
        Base::operator=(std::forward<decltype(value)>(value));
      }
      return *this;
    }

    template <typename Out>
      requires std::is_constructible_v<Out, T> or std::is_convertible_v<T, Out>
    explicit operator Out() const {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return this->Base::value;
      } else {
        return *this;
      }
    }

    friend T &untagged(const Tagged &tagged) noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return tagged->Base::value;
      } else {
        return tagged;
      }
    }
    friend T &untagged(Tagged &tagged) noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return tagged.Base::value;
      } else {
        return tagged;
      }
    }
    friend T untagged(const Tagged &&tagged) noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return tagged.Base::value;
      } else {
        return tagged;
      }
    }
    friend T untagged(Tagged &&tagged) noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return std::move(tagged.Base::value);
      } else {
        return std::move(tagged);
      }
    }

   protected:
    constexpr const T &_get_T() const noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return this->Base::value;
      } else {
        return *this;
      }
    }
    constexpr T &_get_T() noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return this->Base::value;
      } else {
        return *this;
      }
    }
    constexpr T _copy_T() const && noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return this->Base::value;
      } else {
        return *this;
      }
    }
    constexpr T _move_T() && noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return std::move(this->Base::value);
      } else {
        return std::move(*this);
      }
    }

   public:
    // Support all available operators of original type

    // Define friend operators inside the class using macros
    DEFINE_BINARY_OPERATOR(shift_left, <<)
    DEFINE_BINARY_OPERATOR(shift_right, >>)
    DEFINE_BINARY_OPERATOR(add, +)
    DEFINE_BINARY_OPERATOR(subtract, -)
    DEFINE_BINARY_OPERATOR(multiply, *)
    DEFINE_BINARY_OPERATOR(divide, /)
    DEFINE_BINARY_OPERATOR(modulus, %)
    DEFINE_BINARY_OPERATOR(bitwise_and, &)
    DEFINE_BINARY_OPERATOR(bitwise_or, |)
    DEFINE_BINARY_OPERATOR(bitwise_xor, ^)
    DEFINE_BINARY_OPERATOR(logical_and, &&)
    DEFINE_BINARY_OPERATOR(logical_or, ||)
    DEFINE_BINARY_OPERATOR(equal, ==)
    DEFINE_BINARY_OPERATOR(not_equal, !=)
    DEFINE_BINARY_OPERATOR(less, <)
    DEFINE_BINARY_OPERATOR(less_equal, <=)
    DEFINE_BINARY_OPERATOR(greater, >)
    DEFINE_BINARY_OPERATOR(greater_equal, >=)

    // Определяем compound assignment операторы как член‑функции
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(plus_assign, +=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(minus_assign, -=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(multiply_assign, *=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(divide_assign, /=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(modulus_assign, %=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(bitwise_and_assign, &=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(bitwise_or_assign, |=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(bitwise_xor_assign, ^=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(shift_left_assign, <<=)
    DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(shift_right_assign, >>=)

    DEFINE_UNARY_OPERATOR(bitwise_not, ~)
    DEFINE_UNARY_OPERATOR(locical_not, !)
    DEFINE_UNARY_OPERATOR(pre_increment, ++)
    DEFINE_UNARY_OPERATOR(pre_decrement, --)

    DEFINE_SUFFIX_OPERATOR(post_increment, ++)
    DEFINE_SUFFIX_OPERATOR(post_decrement, --)
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

#undef DEFINE_UNARY_OPERATOR_CHECK
#undef DEFINE_UNARY_OPERATOR
#undef DEFINE_SUFFIX_OPERATOR
#undef DEFINE_BINARY_OPERATOR_CHECK
#undef DEFINE_BINARY_OPERATOR
#undef DEFINE_COMPOUND_ASSIGNMENT_OPERATOR
#undef DEFINE_COMPOUND_OPERATOR_CHECK
