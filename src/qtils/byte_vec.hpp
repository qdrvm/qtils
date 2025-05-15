/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <span>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <boost/container_hash/hash.hpp>
#include <boost/operators.hpp>

#include <qtils/byte_view.hpp>
#include <qtils/hex.hpp>
#include <qtils/outcome.hpp>
#include <qtils/size_limited_containers.hpp>
#include <qtils/unhex.hpp>

namespace qtils {

  using byte_t = uint8_t;

  /**
   * @class SLBuffer
   * @brief Size-limited dynamic byte buffer with serialization helpers.
   *
   * SLBuffer is a size-limited extension over a std::vector-based container,
   * with additional APIs for appending primitive types and serializing data.
   *
   * @tparam MaxSize Maximum allowed size for this buffer
   * @tparam Allocator Type of allocator to use
   */
  template <size_t MaxSize, typename Allocator = std::allocator<uint8_t>>
  class SLBuffer : public SLVector<uint8_t, MaxSize, Allocator> {
   public:
    using Base = SLVector<uint8_t, MaxSize, Allocator>;

    template <size_t OtherMaxSize>
    using OtherSLBuffer = SLBuffer<OtherMaxSize, Allocator>;

    SLBuffer() = default;

    /// Construct buffer from a base vector (copy or move)
    explicit SLBuffer(const typename Base::Base &other) : Base(other) {}
    SLBuffer(typename Base::Base &&other) : Base(std::move(other)) {}

    /// Construct buffer from another SLBuffer (copy or move)
    template <size_t OtherMaxSize>
    SLBuffer(const OtherSLBuffer<OtherMaxSize> &other) : Base(other) {}

    template <size_t OtherMaxSize>
    SLBuffer(OtherSLBuffer<OtherMaxSize> &&other) : Base(std::move(other)) {}

    /// Construct buffer from a span
    SLBuffer(const std::span<const uint8_t> &s) : Base(s.begin(), s.end()) {}

    /// Construct buffer from an array
    template <size_t N>
    explicit SLBuffer(const std::array<typename Base::value_type, N> &other)
        : Base(other.begin(), other.end()) {}

    /// Construct buffer from raw pointer range
    SLBuffer(const uint8_t *begin, const uint8_t *end) : Base(begin, end) {};

    using Base::Base;
    using Base::operator=;

    /// Reserve additional space in the buffer
    SLBuffer &reserve(size_t size) {
      Base::reserve(size);
      return *this;
    }

    /// Resize the buffer
    SLBuffer &resize(size_t size) {
      Base::resize(size);
      return *this;
    }

    /// Append raw bytes from a span
    SLBuffer &operator+=(const std::span<const uint8_t> &view) {
      return put(view);
    }

    /// Append a uint8_t in binary form
    SLBuffer &putUint8(uint8_t n) {
      SLBuffer::push_back(n);
      return *this;
    }

    /// Append a uint32_t in big-endian form
    SLBuffer &putUint32(uint32_t n) {
      n = htobe32(n);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      const auto *begin = reinterpret_cast<uint8_t *>(&n);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const auto *end = begin + sizeof(n) / sizeof(uint8_t);
      Base::insert(this->end(), begin, end);
      return *this;
    }

    /// Append a uint64_t in big-endian form
    SLBuffer &putUint64(uint64_t n) {
      n = htobe64(n);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      const auto *begin = reinterpret_cast<uint8_t *>(&n);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const auto *end = begin + sizeof(n) / sizeof(uint8_t);
      Base::insert(this->end(), begin, end);
      return *this;
    }

    /// Append a string as bytes
    SLBuffer &put(std::string_view view) {
      Base::insert(Base::end(), view.begin(), view.end());
      return *this;
    }

    /// Append raw bytes from a span
    SLBuffer &put(std::span<const uint8_t> view) {
      Base::insert(Base::end(), view.begin(), view.end());
      return *this;
    }

    /// Access as std::vector (const ref)
    const std::vector<uint8_t> &asVector() const {
      return static_cast<const typename Base::Base &>(*this);
    }

    /// Access as std::vector (ref)
    std::vector<uint8_t> &asVector() {
      return static_cast<typename Base::Base &>(*this);
    }

    /// Extract as std::vector (copy)
    std::vector<uint8_t> toVector() & {
      return static_cast<typename Base::Base &>(*this);
    }

    /// Extract as std::vector (move)
    std::vector<uint8_t> toVector() && {
      return std::move(static_cast<typename Base::Base &>(*this));
    }

    /// Get a sub-buffer (like subspan)
    SLBuffer subbuffer(size_t offset = 0, size_t length = -1) const {
      return SLBuffer(view(offset, length));
    }

    /// Get a view of this buffer
    [[nodiscard]] ByteView view(size_t offset = 0, size_t length = -1) const {
      return std::span(*this).subspan(offset, length);
    }

    /// Compile-time view with fixed offset and length
    template <size_t Offset, size_t Length>
    [[nodiscard]] ByteView view() const {
      return std::span(*this).template subspan<Offset, Length>();
    }

    /// Convert buffer to hex string
    [[nodiscard]] std::string toHex() const {
      return fmt::format("{:xx}", Hex(*this));
    }

    /// Construct buffer from hex string
    static outcome::result<SLBuffer> fromHex(std::string_view hex) {
      return unhex<SLBuffer>(hex);
    }

    /// Convert buffer to raw string (copy)
    [[nodiscard]] std::string toString() const {
      return std::string{Base::cbegin(), Base::cend()};
    }

    /// Get buffer content as string_view
    [[nodiscard]] std::string_view asString() const {
      return std::string_view(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          reinterpret_cast<const char *>(Base::data()),
          Base::size());
    }

    /// Construct buffer from string
    static SLBuffer fromString(const std::string_view &src) {
      return {src.begin(), src.end()};
    }
  };

  /// Output operator for SLBuffer
  template <size_t MaxSize>
  inline std::ostream &operator<<(
      std::ostream &os, const SLBuffer<MaxSize> &buffer) {
    return os << ByteView(buffer);
  }

  /// Type alias for unbounded SLBuffer
  using ByteVec = SLBuffer<std::numeric_limits<size_t>::max()>;

  /// Empty ByteVec constant
  inline static const ByteVec EmptyVec{};
}  // namespace qtils

/// std::hash specialization for SLBuffer
template <size_t N>
struct std::hash<qtils::SLBuffer<N>> {
  size_t operator()(const qtils::SLBuffer<N> &x) const {
    return boost::hash_range(x.begin(), x.end());
  }
};

template <typename Char>
struct fmt::range_format_kind<qtils::ByteVec, Char>
    : std::integral_constant<fmt::range_format, fmt::range_format::disabled> {};

/// fmt::formatter specialization for ByteVec
template <>
struct fmt::formatter<qtils::ByteVec> : fmt::formatter<qtils::Hex> {};
