/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 *  QTILS_UNWRAP - attemps to extract the value from the provided std::expected
 *      expression and assign it to the provided declaration, returns error if fails.
 *  QTILS_UNWRAP_void - same as QTILS_UNWRAP, but no assignment (for std::expected<void, E>).
 */
 
#include <qtils/macro/common.hpp>

#define QTILS_UNWRAP_NAME QTILS_UNIQUE_NAME(unwrap_res_)

#define QTILS_UNWRAP_3(unwrap_res, decl, expr)  \
  auto unwrap_res = (expr);                     \
  if (!unwrap_res) {                            \
    return std::unexpected(unwrap_res.error()); \
  }                                             \
  decl = unwrap_res.value();

#define QTILS_UNWRAP(decl, expr) QTILS_UNWRAP_3(QTILS_UNWRAP_NAME, decl, expr)

#define QTILS_UNWRAP_void_2(unwrap_res, expr)   \
  auto unwrap_res = (expr);                     \
  if (!unwrap_res) {                            \
    return std::unexpected(unwrap_res.error()); \
  }

#define QTILS_UNWRAP_void(expr) QTILS_UNWRAP_void_2(QTILS_UNWRAP_NAME, expr)
