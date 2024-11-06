/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <type_traits>

namespace qtils::cxx23 {
  template <typename _Tp, typename _Up>
  [[nodiscard]]
  constexpr decltype(auto) forward_like(_Up &&__x) noexcept {
    constexpr bool __as_rval = std::is_rvalue_reference_v<_Tp &&>;

    if constexpr (std::is_const_v<std::remove_reference_t<_Tp>>) {
      using _Up2 = std::remove_reference_t<_Up>;
      if constexpr (__as_rval) {
        return static_cast<const _Up2 &&>(__x);
      } else {
        return static_cast<const _Up2 &>(__x);
      }
    } else {
      if constexpr (__as_rval) {
        return static_cast<std::remove_reference_t<_Up> &&>(__x);
      } else {
        return static_cast<_Up &>(__x);
      }
    }
  }
}  // namespace qtils::cxx23
