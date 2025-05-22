/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __has_include
#if __has_include(<gtest/gtest.h>)
#include <gtest/gtest.h>
#define GTEST_FOUND
#endif
#endif

#ifndef GTEST_FOUND
#error "GTests is not found"
#else
#undef GTEST_FOUND
#endif

#include <qtils/error.hpp>
#include <qtils/macro/common.hpp>
#include <qtils/outcome.hpp>

// ---- details -----

// Unique name
#define _OUTCOME_RESULT_UNIQUE_NAME QTILS_UNIQUE_NAME(__outcome_result)

// assert success

#define _ASSERT_OUTCOME_SUCCESS_TRY(_result_, _expression_)               \
  [[maybe_unused]] auto &&_result_ = (_expression_);                      \
  ({                                                                      \
    if (_result_.has_error()) {                                           \
      GTEST_FATAL_FAILURE_("Outcome of: " #_expression_)                  \
          << "  Actual:   Error '" << _result_.error().message() << "'\n" \
          << "Expected:   Success";                                       \
    }                                                                     \
  })

#define _ASSERT_OUTCOME_SUCCESS_3(_result_, _variable_, _expression_) \
  auto &&_variable_ = ({                                              \
    _ASSERT_OUTCOME_SUCCESS_TRY(_result_, _expression_);              \
    std::move(_result_.value());                                      \
  })

#define _ASSERT_OUTCOME_SUCCESS_2(_variable_, _expression_) \
  _ASSERT_OUTCOME_SUCCESS_3(                                \
      _OUTCOME_RESULT_UNIQUE_NAME, _variable_, _expression_)

#define _ASSERT_OUTCOME_SUCCESS_1(_expression_) \
  _ASSERT_OUTCOME_SUCCESS_TRY(_OUTCOME_RESULT_UNIQUE_NAME, _expression_)

// assert failure

#define _ASSERT_OUTCOME_ERROR_1(_expression_)                 \
  ({                                                          \
    if (auto &&result = (_expression_); result.has_value()) { \
      GTEST_FATAL_FAILURE_("Outcome of: " #_expression_)      \
          << "  Actual:   Success\n"                          \
          << "Expected:   Some error";                        \
    }                                                         \
  })

#define _ASSERT_OUTCOME_ERROR_2(_expression_, _error_)                     \
  ({                                                                       \
    auto &&result = (_expression_);                                        \
    if (result.has_error()) {                                              \
      if (result.error() != qtils::asError(_error_)) {                     \
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
  })

// assert outcome exception

#define _ASSERT_THROW_OUTCOME_1(_expression_)                                 \
  try {                                                                       \
    (_expression_);                                                           \
    GTEST_FATAL_FAILURE_("Expected ErrorException, but no exception thrown"); \
  } catch (const qtils::ErrorException &) {                                   \
    SUCCEED();                                                                \
  } catch (...) {                                                             \
    GTEST_FATAL_FAILURE_("Expected ErrorException, but different exception"); \
  }

#define _ASSERT_THROW_OUTCOME_2(_expression_, _expected_code_)                \
  try {                                                                       \
    (_expression_);                                                           \
    GTEST_FATAL_FAILURE_(                                                     \
        "Expected ErrorException with code, but no exception thrown");        \
  } catch (const qtils::ErrorException &e) {                                  \
    if (e.code() != qtils::asError(_expected_code_)) {                        \
      GTEST_FATAL_FAILURE_("ErrorException code mismatch")                    \
          << "  Actual:   '" << e.code().message() << "'\n"                   \
          << "Expected:   '" << make_error_code(_expected_code_).message()    \
          << "'";                                                             \
    }                                                                         \
  } catch (...) {                                                             \
    GTEST_FATAL_FAILURE_("Expected ErrorException, but different exception"); \
  }

// expect success

#define _EXPECT_OUTCOME_SUCCESS_2(_result_, _expression_)                 \
  [[maybe_unused]] auto &&_result_ = (_expression_);                      \
  ({                                                                      \
    if (_result_.has_error()) {                                           \
      GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_)               \
          << "  Actual:   Error '" << _result_.error().message() << "'\n" \
          << "Expected:   Success";                                       \
    }                                                                     \
  })

#define _EXPECT_OUTCOME_SUCCESS_1(_expression_)                           \
  ({                                                                      \
    if (auto &&_result_ = (_expression_); _result_.has_error()) {         \
      GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_)               \
          << "  Actual:   Error '" << _result_.error().message() << "'\n" \
          << "Expected:   Success";                                       \
    }                                                                     \
  })

// expect failure

#define _EXPECT_OUTCOME_ERROR_3(_result_, _expression_, _error_)            \
  [[maybe_unused]] auto &&_result_ = (_expression_);                        \
  ({                                                                        \
    if (_result_.has_error()) {                                             \
      if (_result_.error() != qtils::asError(_error_)) {                    \
        GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_)               \
            << "  Actual:   Error '" << _result_.error().message() << "'\n" \
            << "Expected:   Error '" << make_error_code(_error_).message()  \
            << "'";                                                         \
      }                                                                     \
    } else {                                                                \
      GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_)                 \
          << "  Actual:   Success\n"                                        \
          << "Expected:   Error '" << make_error_code(_error_).message()    \
          << "'";                                                           \
    }                                                                       \
  })

#define _EXPECT_OUTCOME_ERROR_2(_result_, _expression_)     \
  [[maybe_unused]] auto &&_result_ = (_expression_);        \
  ({                                                        \
    if (_result_.has_value()) {                             \
      GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_) \
          << "  Actual:   Success\n"                        \
          << "Expected:   Some error";                      \
    }                                                       \
  })

#define _EXPECT_OUTCOME_ERROR_1(_expression_)                     \
  ({                                                              \
    if (auto &&_result_ = (_expression_); _result_.has_value()) { \
      GTEST_NONFATAL_FAILURE_("Outcome of: " #_expression_)       \
          << "  Actual:   Success\n"                              \
          << "Expected:   Some error";                            \
    }                                                             \
  })

// expect outcome exception

#define _EXPECT_THROW_OUTCOME_1(_expression_)                \
  try {                                                      \
    (_expression_);                                          \
    GTEST_NONFATAL_FAILURE_(                                 \
        "Expected ErrorException, but no exception thrown"); \
  } catch (const qtils::ErrorException &) {                  \
    SUCCEED();                                               \
  } catch (...) {                                            \
    GTEST_NONFATAL_FAILURE_(                                 \
        "Expected ErrorException, but different exception"); \
  }

#define _EXPECT_THROW_OUTCOME_2(_expression_, _expected_code_)             \
  try {                                                                    \
    (_expression_);                                                        \
    GTEST_NONFATAL_FAILURE_(                                               \
        "Expected ErrorException with code, but no exception thrown");     \
  } catch (const qtils::ErrorException &e) {                               \
    if (e.code() != qtils::asError(_expected_code_)) {                     \
      GTEST_NONFATAL_FAILURE_("ErrorException code mismatch")              \
          << "  Actual:   '" << e.code().message() << "'\n"                \
          << "Expected:   '" << make_error_code(_expected_code_).message() \
          << "'";                                                          \
    }                                                                      \
  } catch (...) {                                                          \
    GTEST_NONFATAL_FAILURE_(                                               \
        "Expected ErrorException, but different exception");               \
  }

// ------ Macros -------

/**
 * @brief Checks the success of an expression.
 *
 * @details This macro can take one or two arguments.
 * If two arguments are provided, the result of `_expression_` is stored in
 * `_result_`. It verifies that the result is successful (`has_value()`),
 * otherwise the test fails fatally.
 *
 * @param ... One or two arguments:
 *  - `_expression_` — The expression whose result is being checked.
 *  - `_result_` (optional) — A variable to store the result of the expression.
 *
 * @note Used in tests based on Google Test.
 *
 * @example
 * @code
 * ASSERT_OUTCOME_SUCCESS(result, some_function());
 * ASSERT_OUTCOME_SUCCESS(some_function());
 * @endcode
 */
#define ASSERT_OUTCOME_SUCCESS(...) \
  _GET_MACRO_OF_2(__VA_ARGS__,      \
      _ASSERT_OUTCOME_SUCCESS_2,    \
      _ASSERT_OUTCOME_SUCCESS_1)(__VA_ARGS__)

/**
 * @brief Checks that an expression resulted in an error.
 *
 * @details This macro verifies that the result of `_expression_` contains an
 * error. If the second argument `_error_` is provided, it checks that the error
 * matches the expected one.
 *
 * @param ... One or two arguments:
 *  - `_expression_` — The expression whose result is being checked.
 *  - `_error_` (optional) — The expected error.
 *
 * @note Used in tests based on Google Test.
 *
 * @example
 * @code
 * ASSERT_OUTCOME_ERROR(some_function());
 * ASSERT_OUTCOME_ERROR(some_function(), expected_error);
 * @endcode
 */
#define ASSERT_OUTCOME_ERROR(...) \
  _GET_MACRO_OF_2(__VA_ARGS__,    \
      _ASSERT_OUTCOME_ERROR_2,    \
      _ASSERT_OUTCOME_ERROR_1)(__VA_ARGS__)

/**
 * @brief Asserts that an expression throws ErrorException (optionally with
 * expected error code)
 *
 * @param _expression_ Expression expected to throw
 * @param _expected_code_ (optional) Expected error code
 *
 * @example
 * @code
 * ASSERT_THROW_OUTCOME(func());
 * ASSERT_THROW_OUTCOME(func(), MyErrors::Invalid);
 * @endcode
 */
#define ASSERT_THROW_OUTCOME(...) \
  _GET_MACRO_OF_2(__VA_ARGS__,    \
      _ASSERT_THROW_OUTCOME_2,    \
      _ASSERT_THROW_OUTCOME_1)(__VA_ARGS__)

/**
 * @brief Checks the success of an expression without terminating the test.
 *
 * @details This macro verifies that the result of `_expression_` is successful
 * (`has_value()`). If the result contains an error, the test registers a
 * non-fatal failure.
 *
 * @param ... One or two arguments:
 *  - `_expression_` — The expression whose result is being checked.
 *  - `_result_` (optional) — A variable to store the result of the expression.
 *
 * @note Used in tests based on Google Test.
 *
 * @example
 * @code
 * EXPECT_OUTCOME_SUCCESS(result, some_function());
 * EXPECT_OUTCOME_SUCCESS(some_function());
 * @endcode
 */
#define EXPECT_OUTCOME_SUCCESS(...) \
  _GET_MACRO_OF_2(__VA_ARGS__,      \
      _EXPECT_OUTCOME_SUCCESS_2,    \
      _EXPECT_OUTCOME_SUCCESS_1)(__VA_ARGS__)

/**
 * @brief Checks that an expression resulted in an error without terminating the
 * test.
 *
 * @details This macro verifies that the result of `_expression_` contains an
 * error. If two or three arguments are provided, it allows specifying the
 * expected error `_error_`.
 *
 * @param ... One, two, or three arguments:
 *  - `_expression_` — The expression whose result is being checked.
 *  - `_result_` (optional) — A variable to store the result of the expression.
 *  - `_error_` (optional) — The expected error.
 *
 * @note Used in tests based on Google Test.
 *
 * @example
 * @code
 * EXPECT_OUTCOME_ERROR(some_function());
 * EXPECT_OUTCOME_ERROR(result, some_function());
 * EXPECT_OUTCOME_ERROR(result, some_function(), expected_error);
 * @endcode
 */
#define EXPECT_OUTCOME_ERROR(...) \
  _GET_MACRO_OF_3(__VA_ARGS__,    \
      _EXPECT_OUTCOME_ERROR_3,    \
      _EXPECT_OUTCOME_ERROR_2,    \
      _EXPECT_OUTCOME_ERROR_1)(__VA_ARGS__)

/**
 * @brief Expects that an expression throws ErrorException (optionally with
 * expected error code)
 *
 * @param _expression_ Expression expected to throw
 * @param _expected_code_ (optional) Expected error code
 *
 * @example
 * @code
 * EXPECT_THROW_OUTCOME(func());
 * EXPECT_THROW_OUTCOME(func(), MyErrors::Conflict);
 * @endcode
 */
#define EXPECT_THROW_OUTCOME(...) \
  _GET_MACRO_OF_2(__VA_ARGS__,    \
      _EXPECT_THROW_OUTCOME_2,    \
      _EXPECT_THROW_OUTCOME_1)(__VA_ARGS__)

// ------ Obsolete ------
// TODO(xDimon): remove it at release

#define _DEPRECATED_MACRO(MSG)                                \
  ({                                                          \
    [[deprecated(MSG)]] [[maybe_unused]] static constexpr int \
        __deprecated_macro_warning__ = 0;                     \
    (void)__deprecated_macro_warning__;                       \
  })

/**
 * @details This macro is obsolete. Use `ASSERT_OUTCOME_ERROR` instead.
 * @deprecated
 */
#define ASSERT_OUTCOME_SOME_ERROR(_expression_)            \
  _DEPRECATED_MACRO("Use `ASSERT_OUTCOME_ERROR` instead"); \
  _ASSERT_OUTCOME_ERROR_1(_expression_)

/**
 * @details This macro is obsolete. Use `EXPECT_OUTCOME_ERROR` instead.
 * @deprecated
 */
#define EXPECT_OUTCOME_SOME_ERROR(_result_, _expression_)  \
  _DEPRECATED_MACRO("Use `EXPECT_OUTCOME_ERROR` instead"); \
  _EXPECT_OUTCOME_ERROR_2(_result_, _expression_)

/**
 * @details This macro is obsolete. Use `ASSERT_OUTCOME_SUCCESS` instead.
 * @deprecated
 */
#define ASSERT_OUTCOME_SUCCESS_TRY(_expression_)             \
  _DEPRECATED_MACRO("Use `ASSERT_OUTCOME_SUCCESS` instead"); \
  _ASSERT_OUTCOME_SUCCESS_1(_expression_)

/**
 * @details This macro is obsolete. Use `EXPECT_OUTCOME_SUCCESS` instead.
 * @deprecated
 */
#define EXPECT_OUTCOME_TRUE_1(_expression_)                  \
  _DEPRECATED_MACRO("Use `EXPECT_OUTCOME_SUCCESS` instead"); \
  _EXPECT_OUTCOME_SUCCESS_1(_expression_)

/**
 * @details This macro is obsolete. Use `ASSERT_OUTCOME_SUCCESS` instead.
 * @deprecated
 */
#define EXPECT_OUTCOME_TRUER(_result_, _expression_)         \
  _DEPRECATED_MACRO("Use `ASSERT_OUTCOME_SUCCESS` instead"); \
  _ASSERT_OUTCOME_SUCCESS_2(_result_, _expression_)

/**
 * @details This macro is obsolete. Use `EXPECT_OUTCOME_ERROR` instead.
 * @deprecated
 */
#define EXPECT_OUTCOME_FALSE_1(_expression_)               \
  _DEPRECATED_MACRO("Use `EXPECT_OUTCOME_ERROR` instead"); \
  _EXPECT_OUTCOME_ERROR_1(_expression_)

/**
 * @details This macro is obsolete. Use `ASSERT_OUTCOME_SUCCESS` instead.
 * @deprecated
 */
#define ASSERT_OUTCOME_SUCCESS_void(_expression_)            \
  _DEPRECATED_MACRO("Use `ASSERT_OUTCOME_SUCCESS` instead"); \
  _ASSERT_OUTCOME_SUCCESS_1(_expression_)
