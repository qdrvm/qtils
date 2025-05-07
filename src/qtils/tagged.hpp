/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Tagged type implementation with operator overloading.
 *
 * This file defines a template class that wraps an underlying type T with an
 * associated tag. It provides overloaded operators to mimic the behavior of T
 * while enforcing type-safety.
 */

#pragma once

#include <algorithm>
#include <string_view>
#include <type_traits>

/**
 * @def DEFINE_UNARY_OPERATOR
 * @brief Macro to define a unary operator overload for a Tagged type.
 *
 * This macro defines member function overloads for the specified unary operator
 * (both lvalue and rvalue versions) as well as corresponding free function
 * overloads. The operator is applied to the underlying value extracted via the
 * `untagged` function.
 *
 * @param op_name A symbolic name for the operator (for readability).
 * @param op The unary operator to be defined.
 */
#define DEFINE_UNARY_OPERATOR(op_name, op)                   \
  /* operators as member-functions */                        \
  decltype(auto) operator op() const &                       \
    requires requires { untagged(*this).operator op(); }     \
  {                                                          \
    return untagged(*this).operator op();                    \
  }                                                          \
  decltype(auto) operator op() &                             \
    requires requires { op untagged(*this).operator op(); }  \
  {                                                          \
    return untagged(*this).operator op();                    \
  }                                                          \
  decltype(auto) operator op() const &&                      \
    requires requires { untagged(*this).operator op(); }     \
  {                                                          \
    return untagged(*this).operator op();                    \
  }                                                          \
  decltype(auto) operator op() &&                            \
    requires requires { untagged(*this).operator op(); }     \
  {                                                          \
    return untagged(*this).operator op();                    \
  }                                                          \
                                                             \
  /* operators as free functions */                          \
  template <typename ARG>                                    \
    requires(std::is_same_v<ARG, Tagged>)                    \
  friend constexpr decltype(auto) operator op(const ARG &a)  \
    requires requires { op untagged(a); }                    \
  {                                                          \
    return op untagged(a);                                   \
  }                                                          \
  template <typename ARG>                                    \
    requires(std::is_same_v<ARG, Tagged>)                    \
  friend constexpr decltype(auto) operator op(ARG &a)        \
    requires requires { op untagged(a); }                    \
  {                                                          \
    return op untagged(a);                                   \
  }                                                          \
  template <typename ARG>                                    \
    requires(std::is_same_v<ARG, Tagged>)                    \
  friend constexpr decltype(auto) operator op(const ARG &&a) \
    requires requires { op untagged(a); }                    \
  {                                                          \
    return op untagged(a);                                   \
  }                                                          \
  template <typename ARG>                                    \
    requires(std::is_same_v<ARG, Tagged>)                    \
  friend constexpr decltype(auto) operator op(ARG &&a)       \
    requires requires { op untagged(a); }                    \
  {                                                          \
    return op untagged(a);                                   \
  }

/**
 * @def DEFINE_BINARY_OPERATOR
 * @brief Macro to define a binary operator overload for a Tagged type.
 *
 * This macro generates member function overloads for the binary operator
 * (handling various value categories) and free function overloads that allow
 * the operator to work with non-Tagged left-hand-side operands. The operator is
 * applied to the underlying values, which are extracted via the `untagged`
 * function.
 *
 * @param op_name A symbolic name for the operator (for readability).
 * @param op The binary operator to be defined.
 * @param unary_op_result Way to return result of call unary-operator with
 * underlying value
 * @param binary_op_result Way to return result of call binary-operator with
 * underlying values
 */
#define DEFINE_BINARY_OPERATOR(op_name, op, unary_op_result, binary_op_result) \
  /* operators as member-functions */                                          \
  template <typename RHS>                                                      \
  constexpr decltype(auto) operator op(RHS &&rhs) const &                      \
    requires requires { untagged(*this) op std::forward<RHS>(rhs); }           \
  {                                                                            \
    auto &&result = untagged(*this) op std::forward<RHS>(rhs);                 \
    return unary_op_result;                                                    \
  }                                                                            \
  template <typename RHS>                                                      \
  constexpr decltype(auto) operator op(RHS &&rhs) &                            \
    requires requires { untagged(*this) op std::forward<RHS>(rhs); }           \
  {                                                                            \
    auto &&result = untagged(*this) op std::forward<RHS>(rhs);                 \
    return unary_op_result;                                                    \
  }                                                                            \
  template <typename RHS>                                                      \
  constexpr decltype(auto) operator op(RHS &&rhs) const &&                     \
    requires requires { untagged(*this) op std::forward<RHS>(rhs); }           \
  {                                                                            \
    auto &&result = untagged(*this) op std::forward<RHS>(rhs);                 \
    return unary_op_result;                                                    \
  }                                                                            \
  template <typename RHS>                                                      \
  constexpr decltype(auto) operator op(RHS &&rhs) &&                           \
    requires requires { untagged(*this) op std::forward<RHS>(rhs); }           \
  {                                                                            \
    auto &&result = untagged(*this) op std::forward<RHS>(rhs);                 \
    return unary_op_result;                                                    \
  }                                                                            \
                                                                               \
  /* operators as free functions */                                            \
  template <typename LHS, typename RHS>                                        \
    requires(not is_tagged_v<LHS> and std::same_as<RHS, Tagged>)               \
  friend constexpr decltype(auto) operator op(LHS &&lhs, const RHS &rhs)       \
    requires requires { (std::forward<LHS>(lhs) op untagged(rhs)); }           \
  {                                                                            \
    auto &&result = std::forward<LHS>(lhs) op untagged(rhs);                   \
    return binary_op_result;                                                   \
  }                                                                            \
                                                                               \
  template <typename LHS, typename RHS>                                        \
    requires(not is_tagged_v<LHS> and std::same_as<RHS, Tagged>)               \
  friend constexpr decltype(auto) operator op(LHS &&lhs, RHS &rhs)             \
    requires requires { (std::forward<LHS>(lhs) op untagged(rhs)); }           \
  {                                                                            \
    auto &&result = std::forward<LHS>(lhs) op untagged(rhs);                   \
    return binary_op_result;                                                   \
  };                                                                           \
                                                                               \
  template <typename LHS, typename RHS>                                        \
    requires(not is_tagged_v<LHS> and std::same_as<RHS, Tagged>)               \
  friend constexpr decltype(auto) operator op(LHS &&lhs, const RHS &&rhs)      \
    requires requires { (std::forward<LHS>(lhs) op untagged(rhs)); }           \
  {                                                                            \
    auto &&result = std::forward<LHS>(lhs) op untagged(rhs);                   \
    return binary_op_result;                                                   \
  }                                                                            \
                                                                               \
  template <typename LHS, typename RHS>                                        \
    requires(not is_tagged_v<LHS> and std::same_as<RHS, Tagged>)               \
  friend constexpr auto operator op(LHS &&lhs, RHS &&rhs)                      \
    requires requires { (std::forward<LHS>(lhs) op untagged(rhs)); }           \
  {                                                                            \
    auto &&result = std::forward<LHS>(lhs) op untagged(rhs);                   \
    return binary_op_result;                                                   \
  }

/**
 * @def DEFINE_COMPOUND_ASSIGNMENT_OPERATOR
 * @brief Macro to define a compound assignment operator for a Tagged type.
 *
 * This macro defines a compound assignment operator that applies the
 * corresponding binary operator to the underlying values and assigns the result
 * back to the Tagged object. It uses helper lambdas to handle both Tagged and
 * non-Tagged arguments.
 *
 * @param op_name A symbolic name for the operator (for readability).
 * @param op The compound assignment operator to be defined.
 */
#define DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(op_name, op)                    \
  template <typename R>                                                     \
    requires requires {                                                     \
      std::declval<T &>() op                                                \
      std::declval<detail::tagged::copy_qualifiers_t<R, untagged_t<R>>>();  \
    }                                                                       \
  decltype(auto) operator op(R &&r) {                                       \
    auto &&lhs_val = [&](auto &&arg) -> decltype(auto) {                    \
      if constexpr (is_tagged_v<decltype(arg)>) {                           \
        return static_cast<T>(arg);                                         \
      } else {                                                              \
        return std::forward<decltype(arg)>(arg);                            \
      }                                                                     \
    }(*this);                                                               \
    auto &&rhs_val = [&](auto &&arg) -> decltype(auto) {                    \
      if constexpr (is_tagged_v<decltype(arg)>) {                           \
        return static_cast<untagged_t<std::remove_cvref_t<decltype(arg)>>>( \
            arg);                                                           \
      } else {                                                              \
        return std::forward<decltype(arg)>(arg);                            \
      }                                                                     \
    }(std::forward<R>(r));                                                  \
    if constexpr (requires { lhs_val op rhs_val; }) {                       \
      *this = Tagged<T, Tag>(lhs_val op rhs_val);                           \
    } else {                                                                \
      static_assert([] { return false; }(),                                 \
          "Compound assignment operator " #op                               \
          " is not available for base type");                               \
    }                                                                       \
    return *this;                                                           \
  }

/**
 * @def DEFINE_SUFFIX_OPERATOR
 * @brief Macro to define a suffix operator overload for a Tagged type.
 *
 * This macro generates overloads for suffix operators (e.g., post-increment or
 * post-decrement) as member functions and as corresponding free functions. The
 * operator is applied to the underlying value obtained via `untagged`.
 *
 * @param op_name A symbolic name for the operator (for readability).
 * @param op The suffix operator to be defined.
 */
#define DEFINE_SUFFIX_OPERATOR(op_name, op)                       \
  decltype(auto) operator op(int) const &                         \
    requires requires { untagged(*this) op; }                     \
  {                                                               \
    return untagged(*this) op;                                    \
  }                                                               \
  decltype(auto) operator op(int) &                               \
    requires requires { untagged(*this) op; }                     \
  {                                                               \
    return untagged(*this) op;                                    \
  }                                                               \
  decltype(auto) operator op(int) const &&                        \
    requires requires { untagged(*this) op; }                     \
  {                                                               \
    return untagged(*this) op;                                    \
  }                                                               \
  decltype(auto) operator op(int) &&                              \
    requires requires { untagged(*this) op; }                     \
  {                                                               \
    return untagged(*this) op;                                    \
  }                                                               \
                                                                  \
  /* operators as free functions */                               \
  template <typename ARG>                                         \
    requires(std::same_as<ARG, Tagged>)                           \
  friend constexpr decltype(auto) operator op(const ARG &a, int)  \
    requires requires { untagged(a) op; }                         \
  {                                                               \
    return untagged(a) op;                                        \
  }                                                               \
  template <typename ARG>                                         \
    requires(std::same_as<ARG, Tagged>)                           \
  friend constexpr decltype(auto) operator op(ARG &a, int)        \
    requires requires { untagged(a) op; }                         \
  {                                                               \
    return untagged(a) op;                                        \
  }                                                               \
  template <typename ARG>                                         \
    requires(std::same_as<ARG, Tagged>)                           \
  friend constexpr decltype(auto) operator op(const ARG &&a, int) \
    requires requires { untagged(a) op; }                         \
  {                                                               \
    return untagged(a) op;                                        \
  }                                                               \
  template <typename ARG>                                         \
    requires(std::same_as<ARG, Tagged>)                           \
  friend constexpr decltype(auto) operator op(ARG &&a, int)       \
    requires requires { untagged(a) op; }                         \
  {                                                               \
    return untagged(a) op;                                        \
  }

namespace qtils {
  /**
   * @brief Forward declaration of the Tagged class template.
   *
   * The Tagged class wraps an underlying type T with an associated tag.
   * It enables type safety and operator overloading while mimicking the
   * behavior of T.
   *
   * @tparam T The underlying type.
   * @tparam Tag The tag type used for differentiation.
   */
  template <typename T, typename Tag>
  class Tagged;

  /**
   * @brief Helper struct to determine if a type is a Tagged type.
   *
   * This helper is specialized for the Tagged class.
   */
  template <typename>
  struct is_tagged : std::false_type {};

  /**
   * @brief Specialization of is_tagged for Tagged types.
   *
   * @tparam T The underlying type.
   * @tparam Tag The tag type.
   */
  template <typename T, typename Tag>
  struct is_tagged<Tagged<T, Tag>> : std::true_type {};

  /**
   * @brief Concept to check if a type is a Tagged type.
   *
   * @tparam T The type to check.
   */
  template <typename T>
  concept is_tagged_v = is_tagged<std::remove_cvref_t<T>>::value;

  /**
   * @brief Helper to extract the original type from a Tagged type.
   *
   * For non-Tagged types, this is simply the decayed type. For Tagged types,
   * it extracts the underlying type.
   *
   * @tparam T The type to extract from.
   */
  template <typename T, typename = void>
  struct untagged_ : std::false_type {
    using Type = std::remove_cvref_t<T>;
  };

  /**
   * @brief Specialization of untagged_ for Tagged types.
   *
   * @tparam T The Tagged type.
   */
  template <typename T>
  struct untagged_<T, std::enable_if_t<is_tagged_v<T>>> {
    using Type = typename std::remove_cvref_t<T>::Type;
  };

  /**
   * @brief Alias to get the original underlying type from T.
   *
   * @tparam T The type to process.
   */
  template <typename T>
  using untagged_t = typename untagged_<T>::Type;

  /**
   * @brief Internal implementation details.
   *
   * The detail namespace contains helper classes, concepts, and functions that
   * are not part of the public API.
   */
  namespace detail::tagged {

    /**
     * @brief Concept to check if a type requires wrapping.
     *
     * A type needs a wrapper if it is not a class, array, or void.
     *
     * @tparam T The type to check.
     */
    template <typename T>
    concept NeedsWrapper = not std::is_class_v<T> and not std::is_array_v<T>
        and not std::is_void_v<T>;

    /**
     * @brief Wrapper class for types that need to be wrapped.
     *
     * For types that are not classes (and are not arrays or void), this wrapper
     * provides a way to store the underlying value.
     *
     * @tparam T The type to wrap.
     */
    template <typename T>
    struct Wrapper {
      Wrapper() = default;

      template <typename U>
        requires std::is_convertible_v<U, T>
      explicit Wrapper(U &&untagged) : value(std::forward<U>(untagged)) {}

      /**
       * @brief Constructs the wrapper with the provided arguments.
       *
       * @tparam Args Parameter pack for the arguments.
       * @param args Arguments forwarded to T's constructor.
       */
      template <typename... Args>
        requires std::is_constructible_v<T, Args...>
      explicit Wrapper(Args &&...args) : value(std::forward<Args>(args)...) {}

     protected:
      T value;  ///< The wrapped value.
    };

    /**
     * @brief Compile-time string literal wrapper.
     *
     * This struct allows a string literal to be used as a template parameter.
     *
     * @tparam N The size of the string literal.
     */
    template <std::size_t N>
    struct StringLiteral {
      /**
       * @brief Constructs a StringLiteral from a C-style string.
       *
       * @param str A C-style string literal.
       */
      explicit constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
      }
      /**
       * @brief Returns a std::string_view of the literal (excluding the
       * terminating null).
       *
       * @return std::string_view representing the literal.
       */
      constexpr std::string_view view() const {
        return std::string_view(value, N - 1);  // remove '\0'
      }
      char value[N];  ///< Internal storage for the string literal.
    };

    /**
     * @brief Copies the qualifiers (lvalue, rvalue, const, volatile) from one
     * type to another.
     *
     * @tparam From The type from which to copy qualifiers.
     * @tparam To The base type to which qualifiers are applied.
     */
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
      using type = T4;  ///< The type T4 with copied qualifiers.
    };

    /**
     * @brief Alias template to simplify access to the copied qualifiers type.
     *
     * @tparam From The type from which to copy qualifiers.
     * @tparam To The base type.
     */
    template <typename From, typename To>
    using copy_qualifiers_t = typename copy_qualifiers<From, To>::type;

  }  // namespace detail::tagged

  /**
   * @brief A type-safe wrapper that tags an underlying type with an additional
   * type parameter.
   *
   * The Tagged class wraps an underlying type T with a tag type Tag. It
   * provides overloads for many operators, forwarding the operations to the
   * underlying value. If T is not a class type (or array/void), it is wrapped
   * inside a detail::Wrapper.
   *
   * @tparam T The underlying type.
   * @tparam Tag_ The tag type.
   */
  template <typename T, typename Tag_>
  class Tagged : public std::conditional_t<detail::tagged::NeedsWrapper<T>,
                     detail::tagged::Wrapper<T>,
                     T> {
    /// Indicates whether the underlying type is wrapped.
    static constexpr bool IsWrapped = detail::tagged::NeedsWrapper<T>;
    /// Alias for the base class type.
    using Base = std::conditional_t<IsWrapped, detail::tagged::Wrapper<T>, T>;

   public:
    /// The underlying type.
    using Type = T;
    /// The tag type.
    using Tag = Tag_;

    using Base::operator=;
    using Base::Base;

    /**
     * @brief Constructs a Tagged object by forwarding the argument to the base
     * class.
     *
     * @tparam Arg Type of the argument that must be convertible to the base
     * type.
     * @param arg Argument that is forwarded to the base class constructor.
     */
    template <typename Arg>
      requires std::is_convertible_v<Arg, T>
    explicit Tagged(Arg &&arg) : Base(std::forward<Arg>(arg)) {}

    /**
     * @brief Constructs a Tagged object forwarding the arguments to the base
     * type.
     *
     * @tparam Args Parameter pack for constructor arguments.
     * @param args Arguments forwarded to the base class constructor.
     */
    template <typename... Args>
      requires std::is_constructible_v<Base, Args...>
    explicit Tagged(Args &&...args) : Base(std::forward<Args>(args)...) {}

    /**
     * @brief Assignment operator from any value.
     *
     * Assigns a value to the underlying type.
     *
     * @param value The value to assign.
     * @return Reference to the current object.
     */
    Tagged &operator=(auto &&value) noexcept(
        std::is_nothrow_assignable_v<Base &, decltype(value)>) {
      if constexpr (IsWrapped) {
        this->value = std::forward<T>(value);
      } else if constexpr (std::is_assignable_v<Base &, decltype(value)>) {
        Base::operator=(std::forward<decltype(value)>(value));
      }
      return *this;
    }

    /**
     * @brief Explicit conversion operator to another type.
     *
     * Converts the Tagged object to another type Out, if possible.
     *
     * @tparam Out The target type.
     * @return The converted value.
     */
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

    /**
     * @brief Returns a const reference to the underlying value.
     *
     * @param tagged A const Tagged object.
     * @return A const reference to the underlying T.
     */
    friend const T &untagged(const Tagged &tagged) noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return tagged.Base::value;
      } else {
        return static_cast<const T &>(tagged);
      }
    }

    /**
     * @brief Returns a non-const reference to the underlying value.
     *
     * @param tagged A non-const Tagged object.
     * @return A reference to the underlying T.
     */
    friend T &untagged(Tagged &tagged) noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return tagged.Base::value;
      } else {
        return static_cast<T &>(tagged);
      }
    }

    /**
     * @brief Returns the underlying value by value from a const rvalue.
     *
     * @param tagged A const rvalue Tagged object.
     * @return The underlying T.
     */
    friend T untagged(const Tagged &&tagged) noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return tagged.Base::value;
      } else {
        return static_cast<const T &>(tagged);
      }
    }
    /**
     * @brief Returns the underlying value by value from an rvalue.
     *
     * @param tagged An rvalue Tagged object.
     * @return The underlying T.
     */
    friend T untagged(Tagged &&tagged) noexcept {
      // NOLINTNEXTLINE(readability-else-after-return)
      if constexpr (IsWrapped) {
        return std::move(tagged.Base::value);
      } else {
        return std::move(static_cast<T &>(tagged));
      }
    }

    // Support all available operators of the underlying type

    // Binary operators
    // clang-format off
    DEFINE_BINARY_OPERATOR(shift_left    , << , Tagged(result) , result        )
    DEFINE_BINARY_OPERATOR(shift_right   , >> , Tagged(result) , result        )
    DEFINE_BINARY_OPERATOR(add           , +  , Tagged(result) , static_cast<std::remove_cvref_t<decltype(result)>>(result) )
    DEFINE_BINARY_OPERATOR(subtract      , -  , Tagged(result) , static_cast<std::remove_cvref_t<decltype(result)>>(result) )
    DEFINE_BINARY_OPERATOR(multiply      , *  , Tagged(result) , static_cast<std::remove_cvref_t<decltype(result)>>(result) )
    DEFINE_BINARY_OPERATOR(divide        , /  , Tagged(result) , static_cast<std::remove_cvref_t<decltype(result)>>(result) )
    DEFINE_BINARY_OPERATOR(modulus       , %  , Tagged(result) , static_cast<std::remove_cvref_t<decltype(result)>>(result) )
    DEFINE_BINARY_OPERATOR(bitwise_and   , &  , Tagged(result) , static_cast<std::remove_cvref_t<decltype(result)>>(result) )
    DEFINE_BINARY_OPERATOR(bitwise_or    , |  , Tagged(result) , static_cast<std::remove_cvref_t<decltype(result)>>(result) )
    DEFINE_BINARY_OPERATOR(bitwise_xor   , ^  , Tagged(result) , static_cast<std::remove_cvref_t<decltype(result)>>(result) )
    DEFINE_BINARY_OPERATOR(logical_and   , && , bool(result)   , bool(result)  )
    DEFINE_BINARY_OPERATOR(logical_or    , || , bool(result)   , bool(result)  )
    DEFINE_BINARY_OPERATOR(equal         , == , bool(result)   , bool(result)  )
    DEFINE_BINARY_OPERATOR(not_equal     , != , bool(result)   , bool(result)  )
    DEFINE_BINARY_OPERATOR(less          , <  , bool(result)   , bool(result)  )
    DEFINE_BINARY_OPERATOR(less_equal    , <= , bool(result)   , bool(result)  )
    DEFINE_BINARY_OPERATOR(greater       , >  , bool(result)   , bool(result)  )
    DEFINE_BINARY_OPERATOR(greater_equal , >= , bool(result)   , bool(result)  )
    // clang-format on

    // Compound assignment operators
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

    // Unary operators
    DEFINE_UNARY_OPERATOR(bitwise_not, ~)
    DEFINE_UNARY_OPERATOR(locical_not, !)
    DEFINE_UNARY_OPERATOR(pre_increment, ++)
    DEFINE_UNARY_OPERATOR(pre_decrement, --)

    // Suffix operators
    DEFINE_SUFFIX_OPERATOR(post_increment, ++)
    DEFINE_SUFFIX_OPERATOR(post_increment, --)
  };

  /**
   * @brief Number-based tag marker.
   *
   * This struct can be used as a tag for the Tagged class, parameterized by a
   * number.
   *
   * @tparam Num The numeric tag.
   */
  template <size_t Num>
  struct NumTag {
    /// The numeric tag value.
    static constexpr size_t tag = Num;
  };

  /**
   * @brief String-based tag marker.
   *
   * This struct can be used as a tag for the Tagged class, parameterized by a
   * compile-time string literal.
   *
   * @tparam Str A compile-time string literal wrapped in a
   * detail::StringLiteral.
   */
  template <detail::tagged::StringLiteral Str>
  struct StrTag {
    /// The string tag value.
    static constexpr std::string_view tag = Str.view();
  };

}  // namespace qtils

#undef DEFINE_UNARY_OPERATOR
#undef DEFINE_BINARY_OPERATOR
#undef DEFINE_COMPOUND_ASSIGNMENT_OPERATOR
#undef DEFINE_SUFFIX_OPERATOR
