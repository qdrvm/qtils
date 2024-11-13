/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define IDENTITY(a) a

#define CONCAT__(a, b) a##b
#define CONCAT_(a, b) CONCAT__(a, b)
#define CONCAT(a, b) CONCAT_(IDENTITY(a), IDENTITY(b))

#define QTILS_UNIQUE_NAME(prefix) CONCAT(prefix, __COUNTER__)

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
