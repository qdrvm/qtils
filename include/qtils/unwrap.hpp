/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/macro.hpp>

#define QTILS_UNWRAP(decl, expr)                              \
  auto UNIQUE_NAME(unwrap_res_) = (expr);                     \
  if (!UNIQUE_NAME(unwrap_res_)) {                            \
    return std::unexpected(UNIQUE_NAME(unwrap_res_).error()); \
  }                                                           \
  decl = UNIQUE_NAME(unwrap_res_).value();

#define QTILS_UNWRAP_void(expr)                               \
  auto UNIQUE_NAME(unwrap_res_) = (expr);                     \
  if (!UNIQUE_NAME(unwrap_res_)) {                            \
    return std::unexpected(UNIQUE_NAME(unwrap_res_).error()); \
  }
