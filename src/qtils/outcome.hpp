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

#define _Q_TRY_void(tmp, cb, expr, error_) \
  auto &&tmp = expr;                       \
  if (tmp.has_error()) {                   \
    tmp.error().v.emplace_front(error_);   \
    return cb(std::move(tmp).error());     \
  }
#define _Q_TRY_out(tmp, cb, out, expr, error) \
  _Q_TRY_void(tmp, cb, expr, error) out = std::move(tmp).value()
#define Q_TRYV(expr, error) _Q_TRY_void(OUTCOME_UNIQUE, , expr, error)
#define Q_TRY(out, expr, error) _Q_TRY_out(OUTCOME_UNIQUE, , out, expr, error)
#define Q_CB_TRYV(cb, expr, error) _Q_TRY_void(OUTCOME_UNIQUE, cb, expr, error)
#define Q_CB_TRY(cb, out, expr, error) \
  _Q_TRY_out(OUTCOME_UNIQUE, cb, out, expr, error)

namespace qtils {
  template <class R>
  using Outcome = boost::outcome_v2::basic_result<R,
      Errors,
      boost::outcome_v2::policy::default_policy<R, Errors, void>>;
}  // namespace qtils

// - - - - - - -
// compatibility
// v v v v v v v

#define _OUTCOME_TRY_void(tmp, expr) \
  _Q_TRY_void(tmp, , expr, Q_ERROR("OUTCOME_TRY"))
#define _BOOST_OUTCOME_TRY(tmp, out, expr) \
  _OUTCOME_TRY_void(tmp, expr) out = std::move(tmp).value()
#define _OUTCOME_TRY_out(tmp, out, expr) \
  _BOOST_OUTCOME_TRY(tmp, auto &&out, expr)
#define BOOST_OUTCOME_TRY(out, expr) \
  _BOOST_OUTCOME_TRY(OUTCOME_UNIQUE, out, expr)
#define _OUTCOME_OVERLOAD(_1, _2, s, ...) _OUTCOME_TRY_##s
#define OUTCOME_TRY(...) \
  _OUTCOME_OVERLOAD(__VA_ARGS__, out, void)(OUTCOME_UNIQUE, __VA_ARGS__)

namespace outcome {
  template <class R>
  using result = qtils::Outcome<R>;
  using boost::outcome_v2::success;
}  // namespace outcome
