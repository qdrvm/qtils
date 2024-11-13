/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define IDENTITY(a) a

#define CONCAT__(a, b, c) a##b##c
#define CONCAT_(a, b, c) CONCAT__(a, b, c)
#define CONCAT(a, b, c) CONCAT_(IDENTITY(a), IDENTITY(b), IDENTITY(c))

#define QTILS_UNIQUE_NAME(prefix) CONCAT(prefix, __func__, __LINE__)

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
