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

#define QTILS_UNWRAP(decl, expr)                       \
  auto QTILS_UNWRAP_NAME = (expr);                     \
  if (!QTILS_UNWRAP_NAME) {                            \
    return std::unexpected(QTILS_UNWRAP_NAME.error()); \
  }                                                    \
  decl = QTILS_UNWRAP_NAME.value();

#define QTILS_UNWRAP_void(expr)                        \
  auto QTILS_UNWRAP_NAME = (expr);                     \
  if (!QTILS_UNWRAP_NAME) {                            \
    return std::unexpected(QTILS_UNWRAP_NAME.error()); \
  }
