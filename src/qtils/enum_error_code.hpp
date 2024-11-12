/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <system_error>
#include <type_traits>

namespace qtils {
  template <typename E>
  concept IsEnumErrorCode =
      std::is_enum_v<E> and requires { errorCodeMessage(E{}); };

  template <IsEnumErrorCode E>
  class EnumErrorCategory : public std::error_category {
   public:
    const char *name() const noexcept final {
      return typeid(E).name();
    }
    std::string message(int code) const final {
      return errorCodeMessage(static_cast<E>(code));
    }
    static const EnumErrorCategory<E> &get() {
      static const EnumErrorCategory<E> category;
      return category;
    }

   private:
    EnumErrorCategory() = default;
  };
}  // namespace qtils

template <qtils::IsEnumErrorCode E>
struct std::is_error_code_enum<E> : std::true_type {};

#define _Q_ERROR_CODE_MESSAGE(E, ns, e) std::string ns errorCodeMessage(E e)
#define _Q_MAKE_ERROR_CODE(E)                                           \
  inline std::error_code make_error_code(const E &e) {                  \
    return {static_cast<int>(e), ::qtils::EnumErrorCategory<E>::get()}; \
  }
#define _Q_ENUM_ERROR_CODE(friend_, E)          \
  friend_ inline _Q_ERROR_CODE_MESSAGE(E, , e); \
  friend_ _Q_MAKE_ERROR_CODE(E)                 \
  friend_ inline _Q_ERROR_CODE_MESSAGE(E, , e)
#define Q_ENUM_ERROR_CODE(E) _Q_ENUM_ERROR_CODE(, E)
#define Q_ENUM_ERROR_CODE_FRIEND(E) _Q_ENUM_ERROR_CODE(friend, E)

// - - - - - - -
// compatibility
// v v v v v v v

#define _OUTCOME_HPP_DECLARE_ERROR(friend_, E) \
  friend_ _Q_ERROR_CODE_MESSAGE(E, , );        \
  friend_ _Q_MAKE_ERROR_CODE(E)

#define OUTCOME_HPP_DECLARE_ERROR(ns, E) \
  namespace ns {                         \
    _OUTCOME_HPP_DECLARE_ERROR(, E)      \
  }
#define OUTCOME_CPP_DEFINE_CATEGORY(ns, E, e) _Q_ERROR_CODE_MESSAGE(E, ns::, e)

#define OUTCOME_HPP_DECLARE_ERROR_1(E) _OUTCOME_HPP_DECLARE_ERROR(, E)
#define OUTCOME_HPP_DECLARE_ERROR_FRIEND(E) \
  _OUTCOME_HPP_DECLARE_ERROR(friend, E)
