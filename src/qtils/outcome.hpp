/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <boost/outcome/result.hpp>
#include <qtils/error.hpp>

#define _OUTCOME_UNIQUE_2(x, y) x##y
#define _OUTCOME_UNIQUE(x, y) _OUTCOME_UNIQUE_2(x, y)
#define OUTCOME_UNIQUE _OUTCOME_UNIQUE(outcome_unique_, __COUNTER__)

namespace qtils {
  template <typename R>
  using Outcome = boost::outcome_v2::basic_result<R,
      std::error_code,
      boost::outcome_v2::policy::default_policy<R, std::error_code, void>>;
}  // namespace qtils

// - - - - - - -
// compatibility
// v v v v v v v

#define _OUTCOME_TRY_void(tmp, expr)    \
  auto &&tmp = expr;                    \
  if (tmp.has_error()) {                \
    return std::move(tmp).as_failure(); \
  }
#define _BOOST_OUTCOME_TRY(tmp, out, expr) \
  _OUTCOME_TRY_void(tmp, expr) out = std::move(tmp).value()
#define BOOST_OUTCOME_TRY(out, expr) \
  _BOOST_OUTCOME_TRY(OUTCOME_UNIQUE, out, expr)
#define _OUTCOME_TRY_out(tmp, out, expr) \
  _BOOST_OUTCOME_TRY(tmp, auto &&out, expr)
#define _OUTCOME_OVERLOAD(_1, _2, s, ...) _OUTCOME_TRY_##s
#define OUTCOME_TRY(...) \
  _OUTCOME_OVERLOAD(__VA_ARGS__, out, void)(OUTCOME_UNIQUE, __VA_ARGS__)

namespace outcome {
  template <class R>
  using result = qtils::Outcome<R>;
  using boost::outcome_v2::failure;
  using boost::outcome_v2::success;
}  // namespace outcome
