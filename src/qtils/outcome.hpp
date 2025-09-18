/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <boost/outcome/result.hpp>
#include <boost/outcome/try.hpp>

#include <qtils/error.hpp>
#include <qtils/error_throw.hpp>
#include <qtils/macro/common.hpp>

#define QTILS_OUTCOME_UNIQUE_NAME QTILS_UNIQUE_NAME(outcome_res_)

namespace qtils {
  template <typename T, typename E = std::error_code>
  using Result = boost::outcome_v2::
      basic_result<T, E, boost::outcome_v2::policy::default_policy<T, E, void>>;

}  // namespace qtils

// - - - - - - -
// compatibility
// v v v v v v v

#define _OUTCOME_TRY_void(expr) BOOST_OUTCOME_TRY(expr)
#define _OUTCOME_TRY_out(out, expr) BOOST_OUTCOME_TRY(auto &&out, expr)
#define _OUTCOME_OVERLOAD(_1, _2, s, ...) _OUTCOME_TRY_##s
#define OUTCOME_TRY(...) _OUTCOME_OVERLOAD(__VA_ARGS__, out, void)(__VA_ARGS__)

namespace outcome {
  template <class R>
  using result = qtils::Result<R>;
  using boost::outcome_v2::failure;
  using boost::outcome_v2::success;
}  // namespace outcome

namespace qtils {
  /**
   * @brief error as a boost exception if an outcome result is failure
   * @tparam T outcome error type
   * @param res outcome result
   */
  template <typename T>
  void raise_on_err(const outcome::result<T> &res) {
    if (res.has_error()) {
      raise(res.error());
    }
  }
}  // namespace qtils
