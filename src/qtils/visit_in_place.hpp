/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Provides utilities for visiting std::variant with concept-based
 *        validation and optional return type enforcement.
 *
 * This header defines traits, concepts, and helper types to ensure that
 * visitor overloads are applicable to the variant's alternatives and,
 * optionally, return values convertible to a common result type.
 */

#pragma once

#include <concepts>
#include <type_traits>
#include <utility>
#include <variant>

namespace qtils {

  namespace detail::visit_in_place {

    /**
     * @brief Trait to check if a type is a specialization of std::variant.
     */
    template <typename T>
    struct is_std_variant : std::false_type {};

    template <typename... Ts>
    struct is_std_variant<std::variant<Ts...>> : std::true_type {};

    /**
     * @brief Concept that checks if a type is a std::variant.
     */
    template <typename T>
    concept IsVariant = is_std_variant<std::remove_cvref_t<T>>::value;

    /**
     * @brief Trait to check if a visitor can be invoked with all types of a
     * variant.
     */
    template <typename V, typename... Ts>
    struct is_applicable_visitor;

    template <typename V, typename... Ts>
    struct is_applicable_visitor<V, std::variant<Ts...>> {
      static constexpr bool value =
          ((std::invocable<V, Ts &> or std::invocable<V, const Ts &>
               or std::invocable<V, Ts &&> or std::invocable<V, const Ts &&>)
              or ...);
    };

    /**
     * @brief Concept that checks if a visitor is applicable to a variant.
     */
    template <typename V, typename Var>
    concept ApplicableVisitor =
        is_applicable_visitor<V, std::remove_cvref_t<Var>>::value;

    /**
     * @brief Concept that checks if all visitors are applicable to a variant.
     */
    template <typename Var, typename... Vs>
    concept ApplicableVisitors = (ApplicableVisitor<Vs, Var> and ...);

    /**
     * @brief Concept to check if a visitor returns a value convertible to R.
     */
    template <typename V, typename T, typename R>
    concept ReturnsConvertibleTo = requires(V v, T t) {
      { v(t) } -> std::convertible_to<R>;
    };

    /**
     * @brief Trait to check if a visitor returns R for all variant
     * alternatives.
     */
    template <typename V, typename Var, typename R>
    struct is_visitor_return_convertible;

    template <typename V, typename... Ts, typename R>
    struct is_visitor_return_convertible<V, std::variant<Ts...>, R> {
      static constexpr bool value = (ReturnsConvertibleTo<V, Ts &, R> or ...);
    };

    /**
     * @brief Concept to validate that a visitor returns R for a variant.
     */
    template <typename V, typename Var, typename R>
    concept VisitorReturnsConvertible =
        is_visitor_return_convertible<V, std::remove_cvref_t<Var>, R>::value;

    /**
     * @brief Concept to check if all visitors return values convertible to R.
     */
    template <typename Var, typename R, typename... Vs>
    concept VisitorsReturnConvertible =
        (VisitorReturnsConvertible<Vs, Var, R> and ...);

    /**
     * @brief Wrapper that casts result of a functor to a common return type.
     *
     * @tparam R The desired return type.
     * @tparam F The underlying functor type.
     */
    template <typename R, typename F>
    class cast_lambda {
     public:
      F f;
      explicit cast_lambda(F func) : f(std::move(func)) {}

      template <typename... Args>
        requires requires(F fn, Args &&...args) {
          { fn(std::forward<Args>(args)...) } -> std::convertible_to<R>;
        }
      R operator()(Args &&...args) const {
        return static_cast<R>(f(std::forward<Args>(args)...));
      }
    };

    /**
     * @brief Visitor class composed of multiple callable types.
     */
    template <typename... Fs>
    class visitor : public std::decay_t<Fs>... {
     public:
      using std::decay_t<Fs>::operator()...;
      explicit visitor(Fs &&...fs)
          : std::decay_t<Fs>(std::forward<Fs>(fs))... {}
    };

    /**
     * @brief Constructs a visitor with a fixed return type.
     */
    template <typename R, typename... Fs>
    constexpr auto make_visitor(Fs &&...fs) {
      auto wrap = [](auto &&fn) {
        using Fn = std::decay_t<decltype(fn)>;
        return cast_lambda<R, Fn>{std::forward<Fn>(fn)};
      };
      return visitor<decltype(wrap(fs))...>{wrap(std::forward<Fs>(fs))...};
    }

    /**
     * @brief Constructs a visitor with an inferred return type from the first
     * visitor.
     */
    template <typename Var, typename F, typename... Fs>
      requires IsVariant<Var>
    constexpr auto make_visitor(F &&f, Fs &&...fs) {
      using First = std::decay_t<F>;
      using FirstArg = std::variant_alternative_t<0, std::remove_cvref_t<Var>>;
      using R = decltype(std::declval<First>()(std::declval<FirstArg &>()));

      auto wrap = [](auto &&fn) {
        using Fn = std::decay_t<decltype(fn)>;
        return cast_lambda<R, Fn>{std::forward<Fn>(fn)};
      };

      return visitor{wrap(std::forward<F>(f)), wrap(std::forward<Fs>(fs))...};
    }

    /**
     * @brief Constructs a plain overloaded visitor (no return cast).
     */
    template <typename... Fs>
    constexpr auto make_visitor(Fs &&...fs) {
      return visitor<Fs...>(std::forward<Fs>(fs)...);
    }

  }  // namespace detail::visit_in_place

  /**
   * @brief Applies visitors to a variant, validating applicability.
   *
   * @tparam V Variant type
   * @tparam Vs Visitors
   * @param var Variant to visit
   * @param vs Visitors to apply
   */
  template <typename V, typename... Vs>
    requires detail::visit_in_place::IsVariant<V>
      and detail::visit_in_place::ApplicableVisitors<V, Vs...>
  constexpr decltype(auto) visit_in_place(V &&var, Vs &&...vs) {
    using detail::visit_in_place::make_visitor;
    return std::visit(
        make_visitor<std::remove_cvref_t<V>>(std::forward<Vs>(vs)...),
        std::forward<V>(var));
  }

  /**
   * @brief Applies visitors to a variant with enforced return type.
   *
   * @tparam R Return type
   * @tparam V Variant type
   * @tparam Vs Visitors
   * @param var Variant to visit
   * @param vs Visitors to apply
   * @return Result of type R
   */
  template <typename R, typename V, typename... Vs>
    requires detail::visit_in_place::IsVariant<V>
      and detail::visit_in_place::ApplicableVisitors<V, Vs...>
      and detail::visit_in_place::VisitorsReturnConvertible<V, R, Vs...>
  constexpr R visit_in_place(V &&var, Vs &&...vs) {
    using detail::visit_in_place::make_visitor;
    return std::visit<R>(
        make_visitor<R>(std::forward<Vs>(vs)...), std::forward<V>(var));
  }

}  // namespace qtils
