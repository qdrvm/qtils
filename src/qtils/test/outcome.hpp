/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <gtest/gtest.h>
#include <qtils/macro/common.hpp>
#include <qtils/outcome.hpp>

#define OUTCOME_RESULT_UNIQUE_NAME QTILS_UNIQUE_NAME(__outcome_result)

#define _ASSERT_OUTCOME_SUCCESS_TRY(_result_, _expression_)             \
  auto &&_result_ = (_expression_);                                     \
  if (not(_result_).has_value()) {                                      \
    GTEST_FATAL_FAILURE_("Outcome of: " #_expression_)                  \
        << "  Actual:   Error '" << _result_.error().message() << "'\n" \
        << "Expected:   Success";                                       \
  }

#define _ASSERT_OUTCOME_SUCCESS(_result_, _variable_, _expression_) \
  _ASSERT_OUTCOME_SUCCESS_TRY(_result_, _expression_);              \
  auto &&_variable_ = std::move(_result_.value());

#define ASSERT_OUTCOME_SUCCESS(_variable_, _expression_) \
  _ASSERT_OUTCOME_SUCCESS(OUTCOME_RESULT_UNIQUE_NAME, _variable_, _expression_)

#define ASSERT_OUTCOME_SUCCESS_TRY(_expression_)                           \
  {                                                                        \
    _ASSERT_OUTCOME_SUCCESS_TRY(OUTCOME_RESULT_UNIQUE_NAME, _expression_); \
  }

#define ASSERT_OUTCOME_SOME_ERROR(_expression_)          \
  {                                                      \
    auto &&result = (_expression_);                      \
    if (not result.has_error()) {                        \
      GTEST_FATAL_FAILURE_("Outcome of: " #_expression_) \
          << "  Actual:   Success\n"                     \
          << "Expected:   Some error";                   \
    }                                                    \
  }

#define ASSERT_OUTCOME_ERROR(_expression_, _error_)                        \
  {                                                                        \
    auto &&result = (_expression_);                                        \
    if (result.has_error()) {                                              \
      if (result != outcome::failure(_error_)) {                           \
        GTEST_FATAL_FAILURE_("Outcome of: " #_expression_)                 \
            << "  Actual:   Error '" << result.error().message() << "'\n"  \
            << "Expected:   Error '" << make_error_code(_error_).message() \
            << "'";                                                        \
      }                                                                    \
    } else {                                                               \
      GTEST_FATAL_FAILURE_("Outcome of: " #_expression_)                   \
          << "  Actual:   Success\n"                                       \
          << "Expected:   Error '" << make_error_code(_error_).message()   \
          << "'";                                                          \
    }                                                                      \
  }

#define EXPECT_OUTCOME_SUCCESS(_result_, _expression_)                  \
  [[maybe_unused]] auto &&_result_ = (_expression_);                    \
  if (_result_.has_error()) {                                           \
    GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_)               \
        << "  Actual:   Error '" << _result_.error().message() << "'\n" \
        << "Expected:   Success";                                       \
  }

#define EXPECT_OUTCOME_SOME_ERROR(_result_, _expression_) \
  [[maybe_unused]] auto &&_result_ = (_expression_);      \
  if (not _result_.has_error()) {                         \
    GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_) \
        << "  Actual:   Success\n"                        \
        << "Expected:   Some error";                      \
  }

#define EXPECT_OUTCOME_ERROR(_result_, _expression_, _error_)                  \
  [[maybe_unused]] auto &&_result_ = (_expression_);                           \
  if (_result_.has_error()) {                                                  \
    if (_result_ != outcome::failure(_error_)) {                               \
      GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_)                    \
          << "  Actual:   Error '" << _result_.error().message() << "'\n"      \
          << "Expected:   Error '" << make_error_code(_error_).message()       \
          << "'";                                                              \
    }                                                                          \
  } else {                                                                     \
    GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_)                      \
        << "  Actual:   Success\n"                                             \
        << "Expected:   Error '" << make_error_code(_error_).message() << "'"; \
  }

// TODO(xDimon): Discuss about vitality of macros below,
//  because they works not as remaining gtest macros

#define EXPECT_OK(expr) (expr).value()

#define EXPECT_HAS_ERROR(expr) EXPECT_TRUE((expr).has_error())

#define _EXPECT_EC(tmp, expr, expected) \
  auto &&tmp = (expr);                  \
  EXPECT_HAS_ERROR(tmp);                \
  EXPECT_EQ(tmp.error(), qtils::asError((expected)));
#define EXPECT_EC(expr, expected) _EXPECT_EC(OUTCOME_UNIQUE, (expr), (expected))
