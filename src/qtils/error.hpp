/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <any>
#include <cstdint>
#include <deque>
#include <ranges>
#include <system_error>

#include <fmt/format.h>

#include <qtils/optref.hpp>

template <>
struct fmt::formatter<std::error_code> {
  static constexpr auto parse(format_parse_context &ctx) {
    return ctx.begin();
  }
  static auto format(const std::error_code &error, format_context &ctx) {
    return fmt::format_to(ctx.out(),
        "{}({}) {}",
        error.category().name(),
        error.value(),
        error.message());
  }
};

#define Q_ERROR_LOCATION \
  (::qtils::ErrorLocation{__PRETTY_FUNCTION__, __FILE__, __LINE__})

#define Q_ERROR(error_) (::qtils::Error{Q_ERROR_LOCATION, error_})

namespace qtils::error {
  struct ErrorLocation {
    const char *fn;
    const char *file;
    uint32_t line;
  };

  template <typename E>
  concept IsEnum = std::is_enum_v<E>;

  template <typename E>
  concept MakeErrorCode = requires { make_error_code(E{}); };

  template <IsEnum E>
  fmt::format_context::iterator formatEnum(fmt::format_context &ctx, E error) {
    return fmt::format_to(
        ctx.out(), "{}({})", typeid(E).name(), fmt::underlying(error));
  }

  class Error {
   public:
    Error(const ErrorLocation &location, std::nullptr_t)
        : error_format_{nullptr}, location_{location} {}
    Error(const ErrorLocation &location, const char *error)
        : error_{error},
          error_format_{[](fmt::format_context &ctx, const std::any &error) {
            return fmt::format_to(
                ctx.out(), "{}", std::any_cast<const char *>(error));
          }},
          location_{location} {}
    template <IsEnum E>
    Error(const ErrorLocation &location, E error)
      requires(not MakeErrorCode<E>)
        : error_{error},
          error_format_{[](fmt::format_context &ctx, const std::any &any) {
            auto &error = std::any_cast<const E &>(any);
            if constexpr (fmt::is_formattable<E>::value) {
              return fmt::format_to(ctx.out(), "{}", error);
            }
            return formatEnum(ctx, error);
          }},
          location_{location} {}
    template <IsEnum E>
    Error(const ErrorLocation &location, E error)
      requires(MakeErrorCode<E>)
        : Error{location, make_error_code(error)} {}
    Error(const ErrorLocation &location, const std::error_code &error)
        : error_{error},
          error_format_{[](fmt::format_context &ctx, const std::any &any) {
            return fmt::format_to(
                ctx.out(), "{}", std::any_cast<const std::error_code &>(any));
          }},
          location_{location} {}

    const ErrorLocation &location() const {
      return location_;
    }

    bool isNull() const {
      return error_format_ == nullptr;
    }

    template <typename T>
    OptRef<T> as() {
      return std::any_cast<T>(&error_);
    }

    template <typename T>
    OptRef<const T> as() const {
      return std::any_cast<T>(&error_);
    }

    template <IsEnum E>
    std::optional<E> ec() const {
      if (auto ec = as<std::error_code>()) {
        static auto &category = make_error_code(E{}).category();
        if (ec->category() == category) {
          return static_cast<E>(ec->value());
        }
      }
      return std::nullopt;
    }

    template <IsEnum E>
    bool ec(E e) const {
      return ec<E>() == e;
    }

   protected:
    std::any error_;
    fmt::format_context::iterator (*error_format_)(
        fmt::format_context &, const std::any &);
    ErrorLocation location_;

    friend struct fmt::formatter<Error>;
  };

  struct Errors {
    Errors(Error error) : v{std::move(error)} {}

    template <typename F>
    auto find(const F &f) const {
      auto it = std::find_if(v.begin(), v.end(), f);
      return it == v.end() ? std::nullopt : std::make_optional(it);
    }

    template <typename F>
    auto find(const F &f) {
      auto it = std::find_if(v.begin(), v.end(), f);
      return it == v.end() ? std::nullopt : std::make_optional(it);
    }

    template <IsEnum E>
    bool ec(E e) const {
      return find([&](const Error &error) { return error.ec(e); }).has_value();
    }

    std::deque<Error> v;
  };

  inline auto make_exception_ptr(const Errors &errors) {
    return std::make_exception_ptr(errors);
  }
}  // namespace qtils::error

namespace qtils {
  using error::Error;
  using error::ErrorLocation;
  using error::Errors;
}  // namespace qtils

template <>
struct fmt::formatter<qtils::Error> {
  static constexpr auto parse(format_parse_context &ctx) {
    return ctx.begin();
  }
  static auto format(const qtils::Error &error, format_context &ctx) {
    if (error.isNull()) {
      return ctx.out();
    }
    return error.error_format_(ctx, error.error_);
  }
};

template <>
struct fmt::formatter<qtils::Errors> {
  static constexpr auto parse(format_parse_context &ctx) {
    return ctx.begin();
  }
  static auto format(const qtils::Errors &errors, format_context &ctx) {
    auto r = errors.v | std::views::filter([](const qtils::Error &e) {
      return not e.isNull();
    });
    return fmt::format_to(ctx.out(), "{}", fmt::join(r, "; "));
  }
};
