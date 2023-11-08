/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <system_error>

template <typename E>
  requires(requires { errorCodeMessage(E{}); })
struct std::is_error_code_enum<E> : std::true_type {};

namespace qtils {
  template <typename E>
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

#define _Q_MAKE_ERROR_CODE(E)                                           \
  inline std::error_code make_error_code(const E &e) {                  \
    return {static_cast<int>(e), ::qtils::EnumErrorCategory<E>::get()}; \
  }
#define _Q_ERROR_CODE_MESSAGE(E, ns, e) auto ns errorCodeMessage(const E &e)
#define Q_ENUM_ERROR_CODE(E) \
  _Q_MAKE_ERROR_CODE(E)      \
  inline _Q_ERROR_CODE_MESSAGE(E, , e)

// - - - - - - -
// compatibility
// v v v v v v v

#define OUTCOME_HPP_DECLARE_ERROR(ns, E) \
  namespace ns {                         \
    _Q_MAKE_ERROR_CODE(E)                \
    _Q_ERROR_CODE_MESSAGE(E, , );        \
  }
#define OUTCOME_CPP_DEFINE_CATEGORY(ns, E, e) _Q_ERROR_CODE_MESSAGE(E, ns::, e)
