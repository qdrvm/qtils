/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <system_error>

#include <boost/throw_exception.hpp>

namespace qtils {
  using ErrorException = boost::wrapexcept<std::system_error>;

  /**
   * @brief throws outcome::result error as a boost exception
   * @tparam T enum error type, only outcome::result enums are allowed
   * @param t error value
   */
  template <typename T>
    requires std::is_enum_v<T>
  void raise(T t) {
    std::error_code ec = make_error_code(t);
    boost::throw_exception(std::system_error(ec));
  }

  /**
   * @brief throws outcome::result error made of error as boost exception
   * @tparam T outcome error type
   * @param t outcome error value
   */
  template <typename T>
    requires(not std::is_enum_v<T>)
  void raise(const T &t) {
    boost::throw_exception(std::system_error(t.value(), t.category()));
  }

}  // namespace qtils
