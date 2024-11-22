/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <type_traits>
#include <version>

//
// Placeholder while qtils is used in projects without C++23 support
//

namespace qtils::cxx23 {

#if defined(__cpp_lib_forward_like) && __cpp_lib_forward_like >= 202207L

  using std::forward_like;

#else
  template <typename T, typename U>
  [[nodiscard]]
  constexpr decltype(auto) forward_like(U &&x) noexcept {
    constexpr bool as_rval = std::is_rvalue_reference_v<T &&>;

    if constexpr (std::is_const_v<std::remove_reference_t<T> >) {
      using U2 = std::remove_reference_t<U>;
      if constexpr (as_rval) {
        return static_cast<const U2 &&>(x);
      } else {
        return static_cast<const U2 &>(x);
      }
    } else {
      if constexpr (as_rval) {
        return static_cast<std::remove_reference_t<U> &&>(x);
      } else {
        return static_cast<U &>(x);
      }
    }
  }
#endif

}  // namespace qtils::cxx23
