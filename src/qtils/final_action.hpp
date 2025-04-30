/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>
#include <optional>
#include <utility>

#include <qtils/option_take.hpp>

namespace qtils {

  /**
   * @brief A class that executes a given function upon destruction.
   *
   * This class is useful for ensuring that a specific action is performed
   * when a scope is exited.
   *
   * @tparam F The type of the function to be executed.
   */
  template <typename F>
  struct FinalAction {
    FinalAction() = delete;
    FinalAction(FinalAction &&func) = delete;
    FinalAction(const FinalAction &func) = delete;
    FinalAction &operator=(FinalAction &&func) = delete;
    FinalAction &operator=(const FinalAction &func) = delete;

    /**
     * @brief Constructs a FinalAction with a function to execute on
     * destruction.
     * @param func The function to execute.
     */
    FinalAction(F &&func) : func(std::forward<F>(func)) {}

    /**
     * @brief Destructor that executes the stored function.
     */
    ~FinalAction() {
      func();
    }

    // Prevent dynamic allocation
    void *operator new(size_t) = delete;
    void *operator new(size_t, void *) = delete;
    void *operator new[](size_t) = delete;
    void *operator new[](size_t, void *) = delete;

   private:
    F func;  ///< The function to execute on destruction.
  };

  /**
   * @brief Deduction guide for FinalAction.
   */
  template <typename F>
  FinalAction(F &&) -> FinalAction<F>;

  /**
   * @brief A class that executes a given function upon destruction but can be
   * moved.
   *
   * This class is similar to FinalAction but allows move operations.
   *
   * @tparam F The type of the function to be executed.
   */
  template <typename F>
  struct MovableFinalAction {
    MovableFinalAction() = delete;
    MovableFinalAction(MovableFinalAction &&func)
        : func{qtils::optionTake(func.func)} {}
    MovableFinalAction(const MovableFinalAction &func) = delete;
    MovableFinalAction &operator=(MovableFinalAction &&func) = delete;
    MovableFinalAction &operator=(const MovableFinalAction &func) = delete;

    /**
     * @brief Constructs a MovableFinalAction with a function to execute on
     * destruction.
     * @param func The function to execute.
     */
    MovableFinalAction(F &&func) : func(std::forward<F>(func)) {}

    /**
     * @brief Destructor that executes the stored function if it exists.
     */
    ~MovableFinalAction() {
      if (func.has_value()) {
        func->operator()();
      }
    }

   private:
    /// The function to execute on destruction (if present).
    std::optional<F> func;
  };

  /**
   * @brief Deduction guide for MovableFinalAction.
   */
  template <typename F>
  MovableFinalAction(F &&) -> MovableFinalAction<F>;

}  // namespace qtils
