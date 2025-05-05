/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <type_traits>

#include <qtils/byte_vec.hpp>

namespace qtils {

  /**
   * @class ByteVecOrView
   * @brief Represents either an owned buffer or a non-owning view.
   *
   * This class is a lightweight wrapper around either a read-only view
   * (`ByteView`) or a movable/owned buffer (`ByteVec`).
   * It supports implicit conversion to `ByteView` and can lazily
   * promote a view into a mutable buffer.
   */
  class ByteVecOrView {
    using Span = std::span<const uint8_t>;

    template <typename T>
    using AsSpan = std::enable_if_t<std::is_convertible_v<T, Span>>;

    struct Moved {};  ///< Tag type indicating moved-from state

   public:
    ByteVecOrView() = default;
    ~ByteVecOrView() = default;

    /// Construct from a non-owning view
    ByteVecOrView(const ByteView &view) : variant{view} {}

    /// Construct from a static array
    template <size_t N>
    ByteVecOrView(const std::array<uint8_t, N> &array)
        : variant{ByteView(array)} {}

    /// Deleted: cannot construct from lvalue vector
    ByteVecOrView(const std::vector<uint8_t> &vector) = delete;

    /// Construct from rvalue vector
    ByteVecOrView(std::vector<uint8_t> &&vector)
        : variant{ByteVec{std::move(vector)}} {}

    ByteVecOrView(const ByteVecOrView &) = delete;
    ByteVecOrView(ByteVecOrView &&) noexcept = default;

    ByteVecOrView &operator=(const ByteVecOrView &) = delete;
    ByteVecOrView &operator=(ByteVecOrView &&) = default;

    /// Checks if buffer is owned
    bool isOwned() const {
      if (variant.index() == 2) {
        throw std::logic_error{"Tried to use moved ByteVecOrView"};
      }
      return variant.index() == 1;
    }

    /// Get read-only view
    [[nodiscard]] ByteView view() const {
      if (!isOwned()) {
        return std::get<ByteView>(variant);
      }
      return ByteView{std::get<ByteVec>(variant)};
    }

    /// Implicit conversion to ByteView
    operator ByteView() const {
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
    ByteVec &mut() {
      if (!isOwned()) {
        auto view = std::get<ByteView>(variant);
        variant = ByteVec{view};
      }
      return std::get<ByteVec>(variant);
    }

    /// Extract buffer (copy if view, clear internal state)
    ByteVec intoByteVec() & {
      auto buffer = std::move(mut());
      variant = Moved{};
      return buffer;
    }

    /// Extract buffer (move overload)
    ByteVec intoByteVec() && {
      return std::move(mut());
    }

    /// Extract buffer (copy if view, clear internal state)
    [[deprecated("Use ByteVec::intoBuffer() instead")]]  //
    ByteVec
    intoBuffer() & {
      return intoByteVec();
    }

    /// Extract buffer (move overload)
    [[deprecated("Use ByteVec::intoBuffer() instead")]]  //
    ByteVec
    intoBuffer() && {
      return intoByteVec();
    }

   private:
    std::variant<ByteView, ByteVec, Moved> variant;

    /// Equality operator for ByteVecOrView vs span-like object
    template <typename T, typename = AsSpan<T>>
    friend bool operator==(const ByteVecOrView &l, const T &r) {
      return l.view() == Span{r};
    }

    /// Equality operator for span-like object vs ByteVecOrView
    template <typename T, typename = AsSpan<T>>
    friend bool operator==(const T &l, const ByteVecOrView &r) {
      return Span{l} == r.view();
    }
  };

}  // namespace qtils

/// Formatter specialization for ByteVecOrView (reuses ByteView)
template <>
struct fmt::formatter<qtils::ByteVecOrView> : fmt::formatter<qtils::ByteView> {
};
