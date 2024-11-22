/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <format>
#include <optional>
#include <string_view>

#include <source_location>

namespace qtils {

  /**
   * std::optional-like class that contains a pointer to some object
   */
  template <typename T>
  class OptionalRef {
   public:
    OptionalRef(T &value) : value_{&value} {}
    OptionalRef(T &&value) = delete;
    OptionalRef() : value_{} {}
    OptionalRef(std::nullopt_t) : value_{} {}
    OptionalRef(const std::optional<T> &opt) : value_{opt ? &*opt : nullptr} {}
    OptionalRef(std::optional<T> &opt) : value_{opt ? &*opt : nullptr} {}
    template <typename T_ = T>
      requires std::is_const_v<T_>
    OptionalRef(const OptionalRef<std::remove_const_t<T_>> &opt)
        : value_{opt ? &*opt : nullptr} {}

    bool has_value() const {
      return value_ != nullptr;
    }

#if defined(__cpp_explicit_this_parameter) \
    && __cpp_explicit_this_parameter >= 202110L
    template <typename Self>
    decltype(auto) value(this Self &&self) {
      if (self.value_ == nullptr) {
        throw std::bad_optional_access{};
      }
      return *self.value_;
    }

    template <typename Self>
    decltype(auto) operator*(this Self &&self) {
      QTILS_ASSERT(self.value_ != nullptr);
      return *self.value_;
    }

    template <typename Self>
    decltype(auto) operator->(this Self &&self) {
      QTILS_ASSERT(self.value_ != nullptr);
      return self.value_;
    }
#else  // delete when C++20 support is dropped
    T &value() {
      if (value_ == nullptr) {
        throw std::bad_optional_access{};
      }
      return *value_;
    }

    const T &value() const {
      if (value_ == nullptr) {
        throw std::bad_optional_access{};
      }
      return *value_;
    }

    T &operator*() {
      QTILS_ASSERT(value_ != nullptr);
      return *value_;
    }

    const T &operator*() const {
      QTILS_ASSERT(value_ != nullptr);
      return *value_;
    }

    T *operator->() {
      QTILS_ASSERT(value_ != nullptr);
      return value_;
    }

    const T *operator->() const {
      QTILS_ASSERT(value_ != nullptr);
      return value_;
    }

#endif

    explicit operator bool() const {
      return has_value();
    }

    template <std::equality_comparable_with<T> U>
    bool operator==(const OptionalRef<U> &other) const {
      return (!has_value() && !other.has_value()) || (*value_ == *other);
    }

   private:
    T *value_;
  };

  template <typename T>
  void expect(const OptionalRef<T> &expected,
      std::string_view expression,
      std::source_location loc = std::source_location::current()) {
    if (!expected) {
      print_and_abort(
          std::format("'{}' does not contain a value", expression), loc);
    }
  }
}  // namespace qtils

template <typename T>
struct std::formatter<qtils::OptionalRef<T>, char> {
  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) {
    auto it = ctx.begin();
    return it;
  }

  template <class FmtContext>
  FmtContext::iterator format(
      qtils::OptionalRef<T> opt, FmtContext &ctx) const {
    auto out = ctx.out();
    if (opt) {
      std::format_to(out, "{}", *opt);
    } else {
      std::format_to(out, "<none>");
    }
    return out;
  }
};
