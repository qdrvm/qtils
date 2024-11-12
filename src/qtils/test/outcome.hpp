/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <qtils/outcome.hpp>

#define EXPECT_OK(expr) (expr).value()

#define EXPECT_HAS_ERROR(expr) EXPECT_TRUE((expr).has_error())

#define _EXPECT_EC(tmp, expr, expected) \
  auto &&tmp = (expr);                  \
  EXPECT_HAS_ERROR(tmp);                \
  EXPECT_EQ(tmp.error(), qtils::asError((expected)));
#define EXPECT_EC(expr, expected) _EXPECT_EC(OUTCOME_UNIQUE, (expr), (expected))
