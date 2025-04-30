/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <type_traits>

#include <qtils/buffer.hpp>

namespace qtils {

  /**
   * @class BufferOrView
   * @brief Represents either an owned buffer or a non-owning view.
   *
   * This class is a lightweight wrapper around either a read-only view
   * (`BufferView`) or a movable/owned buffer (`Buffer`).
   * It supports implicit conversion to `BufferView` and can lazily
   * promote a view into a mutable buffer.
   */
  class BufferOrView {
    using Span = std::span<const uint8_t>;

    template <typename T>
    using AsSpan = std::enable_if_t<std::is_convertible_v<T, Span>>;

    struct Moved {};  ///< Tag type indicating moved-from state

   public:
    BufferOrView() = default;
    ~BufferOrView() = default;

    /// Construct from a non-owning view
    BufferOrView(const BufferView &view) : variant{view} {}

    /// Construct from a static array
    template <size_t N>
    BufferOrView(const std::array<uint8_t, N> &array)
        : variant{BufferView(array)} {}

    /// Deleted: cannot construct from lvalue vector
    BufferOrView(const std::vector<uint8_t> &vector) = delete;

    /// Construct from rvalue vector
    BufferOrView(std::vector<uint8_t> &&vector)
        : variant{Buffer{std::move(vector)}} {}

    BufferOrView(const BufferOrView &) = delete;
    BufferOrView(BufferOrView &&) noexcept = default;

    BufferOrView &operator=(const BufferOrView &) = delete;
    BufferOrView &operator=(BufferOrView &&) = default;

    /// Checks if buffer is owned
    bool isOwned() const {
      if (variant.index() == 2) {
        throw std::logic_error{"Tried to use moved BufferOrView"};
      }
      return variant.index() == 1;
    }

    /// Get read-only view
    [[nodiscard]] BufferView view() const {
      if (!isOwned()) {
        return std::get<BufferView>(variant);
      }
      return BufferView{std::get<Buffer>(variant)};
    }

    /// Implicit conversion to BufferView
    operator BufferView() const {
      return view();
    }

    /// Raw pointer to data (for range compatibility)
    auto data() const {
      return view().data();
    }

    /// Size of underlying buffer/view
    size_t size() const {
      return view().size();
    }

    /// Begin iterator for range compatibility
    auto begin() const {
      return view().begin();
    }

    /// End iterator for range compatibility
    auto end() const {
      return view().end();
    }

    /// Returns mutable buffer reference (creates copy if needed)
    Buffer &mut() {
      if (!isOwned()) {
        auto view = std::get<BufferView>(variant);
        variant = Buffer{view};
      }
      return std::get<Buffer>(variant);
    }

    /// Extract buffer (copy if view, clear internal state)
    Buffer intoBuffer() & {
      auto buffer = std::move(mut());
      variant = Moved{};
      return buffer;
    }

    /// Extract buffer (move overload)
    Buffer intoBuffer() && {
      return std::move(mut());
    }

   private:
    std::variant<BufferView, Buffer, Moved> variant;

    /// Equality operator for BufferOrView vs span-like object
    template <typename T, typename = AsSpan<T>>
    friend bool operator==(const BufferOrView &l, const T &r) {
      return l.view() == Span{r};
    }

    /// Equality operator for span-like object vs BufferOrView
    template <typename T, typename = AsSpan<T>>
    friend bool operator==(const T &l, const BufferOrView &r) {
      return Span{l} == r.view();
    }
  };

}  // namespace qtils

/// Formatter specialization for BufferOrView (reuses BufferView)
template <>
struct fmt::formatter<qtils::BufferOrView> : fmt::formatter<qtils::BufferView> {
};
