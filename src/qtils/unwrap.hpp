/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/unique.hpp>

#define _QTILS_UNWRAP_void(tmp, expr)                    \
  auto &&tmp = expr;                                     \
  if (tmp.has_error()) {                                 \
    return std::unexpected{std::move(tmp).as_failure()}; \
  }
#define _QTILS_UNWRAP_out(tmp, out, expr) \
  _QTILS_UNWRAP_void(tmp, expr) out = std::move(tmp).value()
#define QTILS_UNWRAP(out, expr) _QTILS_UNWRAP_out(QTILS_UNIQUE, out, expr)
#define QTILS_UNWRAP_void(expr) _QTILS_UNWRAP_void(QTILS_UNIQUE, expr)
