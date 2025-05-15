/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <array>

#include <fmt/format.h>
#include <boost/functional/hash.hpp>

#include <qtils/byte_view.hpp>
#include <qtils/hex.hpp>
#include <qtils/outcome.hpp>
#include <qtils/span_adl.hpp>
#include <qtils/unhex.hpp>

/**
 * @def JAM_BLOB_STRICT_TYPEDEF
 * @brief Declares a strict typedef of qtils::ByteArr<N> with additional
 * helpers.
 *
 * This macro defines a strongly typed wrapper over a fixed-size binary blob
 * using a derived class. It includes:
 * - Constructors and assignment operators
 * - Static factory methods: fromString, fromHex, fromHexWithPrefix, fromSpan
 * - Hash and fmt format specializations for the type
 *
 * @param space_name Namespace to contain the new type
 * @param class_name Name of the new blob type
 * @param blob_size Size of the underlying blob in bytes
 */
#define JAM_BLOB_STRICT_TYPEDEF(space_name, class_name, blob_size)             \
  namespace space_name {                                                       \
    struct class_name : public ::qtils::ByteArr<blob_size> {                   \
      using Base = ::qtils::ByteArr<blob_size>;                                \
                                                                               \
      class_name() = default;                                                  \
      class_name(const class_name &) = default;                                \
      class_name(class_name &&) = default;                                     \
      class_name &operator=(const class_name &) = default;                     \
      class_name &operator=(class_name &&) = default;                          \
                                                                               \
      explicit class_name(const Base &blob) : Base{blob} {}                    \
      explicit class_name(Base &&blob) : Base{std::move(blob)} {}              \
                                                                               \
      ~class_name() = default;                                                 \
                                                                               \
      class_name &operator=(const Base &blob) {                                \
        Base::operator=(blob);                                                 \
        return *this;                                                          \
      }                                                                        \
                                                                               \
      class_name &operator=(Base &&blob) {                                     \
        Base::operator=(std::move(blob));                                      \
        return *this;                                                          \
      }                                                                        \
                                                                               \
      static ::outcome::result<class_name> fromString(std::string_view data) { \
        OUTCOME_TRY(blob, Base::fromString(data));                             \
        return class_name{std::move(blob)};                                    \
      }                                                                        \
                                                                               \
      static ::outcome::result<class_name> fromHex(std::string_view hex) {     \
        OUTCOME_TRY(blob, Base::fromHex(hex));                                 \
        return class_name{std::move(blob)};                                    \
      }                                                                        \
                                                                               \
      static ::outcome::result<class_name> fromHexWithPrefix(                  \
          std::string_view hex) {                                              \
        OUTCOME_TRY(blob, Base::fromHexWithPrefix(hex));                       \
        return class_name{std::move(blob)};                                    \
      }                                                                        \
                                                                               \
      static ::outcome::result<class_name> fromSpan(                           \
          const ::qtils::ByteView &span) {                                     \
        OUTCOME_TRY(blob, Base::fromSpan(span));                               \
        return class_name{std::move(blob)};                                    \
      }                                                                        \
    };                                                                         \
  };                                                                           \
                                                                               \
  template <>                                                                  \
  struct std::hash<space_name::class_name> {                                   \
    auto operator()(const space_name::class_name &key) const {                 \
      /* NOLINTNEXTLINE */                                                     \
      return boost::hash_range(key.cbegin(), key.cend());                      \
    }                                                                          \
  };                                                                           \
                                                                               \
  template <>                                                                  \
  struct fmt::formatter<space_name::class_name>                                \
      : fmt::formatter<space_name::class_name::Base> {                         \
    template <typename FormatCtx>                                              \
    auto format(const space_name::class_name &blob, FormatCtx &ctx) const      \
        -> decltype(ctx.out()) {                                               \
      return fmt::formatter<space_name::class_name::Base>::format(blob, ctx);  \
    }                                                                          \
  };

namespace qtils {

  /**
   * @enum ByteArrError
   * @brief Error codes for ByteArr creation/initialization
   */
  enum class ByteArrError : uint8_t {
    INCORRECT_LENGTH = 1  ///< Input size does not match the required blob size
  };

  using byte_t = uint8_t;

  /**
   * @class ByteArr
   * @brief Fixed-size binary buffer abstraction with utility methods.
   *
   * Provides size-bound operations, safe construction from various sources,
   * and conversion to/from string and hex representations.
   *
   * @tparam size_ Fixed size of the blob in bytes
   */
  template <size_t size_>
  class ByteArr : public std::array<byte_t, size_> {
    using Array = std::array<byte_t, size_>;

   public:
    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    using const_narref = const byte_t (&)[size_];
    using const_narptr = const byte_t (*)[size_];
    // NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

    /// Default constructor initializes to zero
    constexpr ByteArr() : Array{} {}

    /// Initializes blob from initializer list (truncates if too long)
    constexpr ByteArr(std::initializer_list<byte_t> list) {
      std::copy(list.begin(), list.end(), this->begin());
    }

    /// Access internal array reference (non-owning)
    const_narref internal_array_reference() const {
      return *const_narptr(this->data());
    }

    /// Initializes from std::array<byte_t, size_>
    constexpr explicit ByteArr(const Array &l) : Array{l} {}

    /// Get size of the blob (in bytes)
    static constexpr size_t size() {
      return size_;
    }

    /// Convert blob content to raw std::string
    [[nodiscard]] std::string toString() const {
      return std::string{this->begin(), this->end()};
    }

    /// Convert blob content to hexadecimal string
    [[nodiscard]] std::string toHex() const {
      return fmt::format("{:xx}", Hex(*this));
    }

    /**
     * Construct blob from string content
     * @param data string with exactly `size_` bytes
     * @return ByteArr if successful or error if size mismatches
     */
    static outcome::result<ByteArr> fromString(std::string_view data) {
      if (data.size() != size_) {
        return ByteArrError::INCORRECT_LENGTH;
      }
      ByteArr result;
      std::ranges::copy(data, result.begin());
      return result;
    }

    /**
     * Construct blob from hex-encoded string
     * @param hex string with 2*size_ characters
     * @return ByteArr if hex is valid and of correct length
     */
    static outcome::result<ByteArr> fromHex(std::string_view hex) {
      OUTCOME_TRY(res, unhex<ByteArr>(hex));
      return fromSpan(res);
    }

    /**
     * Construct blob from hex string prefixed with 0x
     * @param hex string prefixed with "0x"
     * @return ByteArr if hex is valid and of correct length
     */
    static outcome::result<ByteArr> fromHexWithPrefix(std::string_view hex) {
      OUTCOME_TRY(res, unhex0x<ByteArr>(hex));
      return fromSpan(res);
    }

    /**
     * Construct blob from binary span
     * @param span binary view
     * @return ByteArr if span is of correct length
     */
    static outcome::result<ByteArr> fromSpan(std::span<const uint8_t> span) {
      if (span.size() != size_) {
        return ByteArrError::INCORRECT_LENGTH;
      }
      ByteArr blob;
      std::ranges::copy(span, blob.begin());
      return blob;
    }

    /// 3-way comparison operator
    auto operator<=>(const ByteArr<size_> &other) const {
      return SpanAdl{*this} <=> other;
    }

    /// Equality operator
    bool operator==(const ByteArr<size_> &other) const {
      return SpanAdl{*this} == other;
    }
  };

  /**
   * @deprecated Use ByteArr<N> instead.
   */
  template <size_t N>
  using BytesN [[deprecated("Use ByteArr<N> instead")]] = ByteArr<N>;

  /// Output operator to stream as hex string
  template <size_t N>
  inline std::ostream &operator<<(std::ostream &os, const ByteArr<N> &blob) {
    return os << blob.toHex();
  }

}  // namespace qtils

/// std::hash specialization for ByteArr<N>
template <size_t N>
struct std::hash<qtils::ByteArr<N>> {
  auto operator()(const qtils::ByteArr<N> &blob) const {
    return boost::hash_range(blob.data(), blob.data() + N);  // NOLINT
  }
};

template <size_t N, typename Char>
struct fmt::range_format_kind<qtils::ByteArr<N>, Char>
    : std::integral_constant<fmt::range_format, fmt::range_format::disabled> {};

/// fmt::formatter specialization for ByteArr<N>
template <size_t N>
struct fmt::formatter<qtils::ByteArr<N>> : fmt::formatter<qtils::Hex> {};

/// Declares outcome error code support for ByteArrError
OUTCOME_HPP_DECLARE_ERROR(qtils, ByteArrError);

inline OUTCOME_CPP_DEFINE_CATEGORY(qtils, ByteArrError, e) {
  using qtils::ByteArrError;

  switch (e) {
    case ByteArrError::INCORRECT_LENGTH:
      return "Input string has incorrect length, not matching the blob size";
  }

  return "Unknown error";
}
