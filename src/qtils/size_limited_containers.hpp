/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <fmt/format.h>
#include <limits>
#include <qtils/cxx20/lexicographical_compare_three_way.hpp>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace qtils {

  /**
   * @brief Concept to check if T is a mutable iterator of Container.
   */
  template <typename Container, typename T>
  concept MutIter = std::is_same_v<T, typename Container::iterator>;

  /**
   * @brief Concept to check if T is a const iterator of Container.
   */
  template <typename Container, typename T>
  concept ConstIter = std::is_same_v<T, typename Container::const_iterator>;

  /**
   * @brief Concept to check if T is either a mutable or const iterator of
   * Container.
   */
  template <typename Container, typename T>
  concept Iter = MutIter<Container, T> or ConstIter<Container, T>;

  /**
   * @brief Exception thrown when a container exceeds its maximum allowed size.
   */
  class MaxSizeException : public std::length_error {
   public:
    /**
     * @brief Constructs MaxSizeException with a formatted message.
     * @tparam Format Format string type
     * @tparam Args Arguments for format string
     */
    template <typename Format, typename... Args>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
    MaxSizeException(const Format &format, const Args &...args)
        : std::length_error(
              fmt::vformat(format, fmt::make_format_args(args...))) {}
  };

  /**
   * @brief STL-compatible container wrapper with size enforcement.
   *
   * Provides runtime checks against a compile-time maximum size limit
   * during construction, assignment, and mutation.
   */
  template <typename BaseContainer, std::size_t MaxSize>
  class SizeLimitedContainer : public BaseContainer {
   protected:
    using Base = BaseContainer;
    using Span = std::span<typename Base::value_type>;

    static constexpr bool size_check_is_enabled =
        MaxSize < std::numeric_limits<typename Base::size_type>::max();

   public:
    // Next line is required at least for the scale-codec
    static constexpr bool is_static_collection = false;

    /**
     * @brief Returns the maximum allowed size of the container.
     */
    [[nodiscard]] inline constexpr typename Base::size_type max_size() {
      return MaxSize;
    }

    /**
     * @brief Default constructor.
     */
    SizeLimitedContainer() = default;

    /**
     * @brief Constructs container with given size.
     * @throws MaxSizeException if size exceeds MaxSize.
     */
    explicit SizeLimitedContainer(size_t size)
        : Base([&] {
            if constexpr (size_check_is_enabled) {
              [[unlikely]] if (size > max_size()) {
                throw MaxSizeException(
                    "Destination has limited size by {}; requested size is {}",
                    max_size(),
                    size);
              }
            }
            return Base(size);
          }()) {}

    /**
     * @brief Constructs container with repeated values.
     * @throws MaxSizeException if size exceeds MaxSize.
     */
    SizeLimitedContainer(size_t size, const typename Base::value_type &value)
        : Base([&] {
            if constexpr (size_check_is_enabled) {
              [[unlikely]] if (size > max_size()) {
                throw MaxSizeException(
                    "Destination has limited size by {}; requested size is {}",
                    max_size(),
                    size);
              }
            }
            return Base(size, value);
          }()) {}

    /**
     * @brief Constructs container from another base container.
     * @throws MaxSizeException if size exceeds MaxSize.
     */
    SizeLimitedContainer(const Base &other)
        : Base([&] {
            if constexpr (size_check_is_enabled) {
              [[unlikely]] if (other.size() > max_size()) {
                throw MaxSizeException(
                    "Destination has limited size by {}; Source size is {}",
                    max_size(),
                    other.size());
              }
            }
            return other;
          }()) {}

    /**
     * @brief Move-constructs container from another base container.
     * @throws MaxSizeException if size exceeds MaxSize.
     */
    SizeLimitedContainer(Base &&other)
        : Base([&] {
            if constexpr (size_check_is_enabled) {
              [[unlikely]] if (other.size() > max_size()) {
                throw MaxSizeException(
                    "Destination has limited size by {}; Source size is {}",
                    max_size(),
                    other.size());
              }
            }
            return std::move(other);
          }()) {}

    /**
     * @brief Constructs container from a range.
     * @throws MaxSizeException if size exceeds MaxSize.
     */
    template <typename Iter>
      requires std::is_base_of_v<std::input_iterator_tag,
          typename std::iterator_traits<Iter>::iterator_category>
    SizeLimitedContainer(Iter begin, Iter end)
        : Base([&] {
            if constexpr (size_check_is_enabled) {
              const size_t size = std::distance(begin, end);
              [[unlikely]] if (size > max_size()) {
                throw MaxSizeException(
                    "Container has limited size by {}; Source range size is {}",
                    max_size(),
                    Base::size());
              }
            }
            return Base(std::move(begin), std::move(end));
          }()) {}

    /**
     * @brief Constructs container from an initializer list.
     * @param list The list of values to initialize from.
     * @throws MaxSizeException if the list size exceeds MaxSize.
     */
    SizeLimitedContainer(std::initializer_list<typename Base::value_type> list)
        : SizeLimitedContainer(list.begin(), list.end()) {}

    /**
     * @brief Assigns values from another container.
     * @param other Container to copy values from.
     * @return Reference to this container.
     * @throws MaxSizeException if the source size exceeds MaxSize.
     */
    SizeLimitedContainer &operator=(const Base &other) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (other.size() > max_size()) {
          throw MaxSizeException(
              "Destination has limited size by {}; Source size is {}",
              max_size(),
              other.size());
        }
      }
      static_cast<Base &>(*this) = other;
      return *this;
    }

    /**
     * @brief Move-assigns values from another container.
     * @param other Container to move from.
     * @return Reference to this container.
     * @throws MaxSizeException if the source size exceeds MaxSize.
     */
    SizeLimitedContainer &operator=(Base &&other) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (other.size() > max_size()) {
          throw MaxSizeException(
              "Destination has limited size by {}; Source size is {}",
              max_size(),
              other.size());
        }
      }
      static_cast<Base &>(*this) = std::move(other);
      return *this;
    }

    /**
     * @brief Assigns values from an initializer list.
     * @param list The initializer list to assign from.
     * @return Reference to this container.
     * @throws MaxSizeException if the list size exceeds MaxSize.
     */
    SizeLimitedContainer &operator=(
        std::initializer_list<typename Base::value_type> list) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (list.size() > max_size()) {
          throw MaxSizeException(
              "Destination has limited size by {}; Source size is {}",
              max_size(),
              list.size());
        }
      }
      static_cast<Base &>(*this) =
          std::forward<std::initializer_list<typename Base::value_type>>(list);
      return *this;
    }

    /**
     * @brief Assigns container with a given size and value.
     * @param size Number of elements to assign.
     * @param value Value to fill with.
     * @throws MaxSizeException if size exceeds MaxSize.
     */
    void assign(
        typename Base::size_type size, const typename Base::value_type &value) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (size > max_size()) {
          throw MaxSizeException(
              "Destination has limited size by {}; Requested size is {}",
              max_size(),
              size);
        }
      }
      return Base::assign(size, value);
    }

    /**
     * @brief Assigns container from a range.
     * @tparam Iter Input iterator type
     * @param begin Start of range
     * @param end End of range
     * @throws MaxSizeException if range exceeds MaxSize.
     */
    template <typename Iter>
      requires std::is_base_of_v<std::input_iterator_tag,
          typename std::iterator_traits<Iter>::iterator_category>
    void assign(Iter begin, Iter end) {
      if constexpr (size_check_is_enabled) {
        const size_t size = std::distance(begin, end);
        [[unlikely]] if (size > max_size()) {
          throw MaxSizeException(
              "Container has limited size by {}; Source range size is {}",
              max_size(),
              Base::size());
        }
      }
      return Base::assign(std::move(begin), std::move(end));
    }

    /**
     * @brief Assigns container from an initializer list.
     * @param list Initializer list of values
     * @throws MaxSizeException if list size exceeds MaxSize.
     */
    void assign(std::initializer_list<typename Base::value_type> list) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (list.size() > max_size()) {
          throw MaxSizeException(
              "Container has limited size by {}; Source range size is {}",
              max_size(),
              Base::size());
        }
      }
      return Base::assign(std::move(list));
    }

    /**
     * @brief Constructs element at the end in-place.
     * @tparam Args Argument pack for construction
     * @return Reference to the created element
     * @throws MaxSizeException if container is full.
     */
    template <typename... Args>
    typename Base::reference &emplace_back(Args &&...args) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (Base::size() >= max_size()) {
          throw MaxSizeException(
              "Container has limited size by {}; Size is already {} ",
              max_size(),
              Base::size());
        }
      }
      return Base::emplace_back(std::forward<Args>(args)...);
    }

    /**
     * @brief Constructs element in-place at given position.
     * @tparam It Iterator type
     * @tparam Args Argument pack for element constructor
     * @param pos Position to insert at
     * @return Iterator to inserted element
     * @throws MaxSizeException if container is full.
     */
    template <typename It, typename... Args>
      requires Iter<Base, It>
    typename Base::iterator emplace(It pos, Args &&...args) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (Base::size() >= max_size()) {
          throw MaxSizeException(
              "Container has limited size by {}; Size is already {} ",
              max_size(),
              Base::size());
        }
      }
      return Base::emplace(std::move(pos), std::forward<Args>(args)...);
    }

    /**
     * @brief Inserts a value at a position.
     * @param pos Iterator to insert position
     * @param value Value to insert
     * @return Iterator to inserted element
     * @throws MaxSizeException if container is full.
     */
    template <typename It, typename... Args>
      requires Iter<Base, It>
    typename Base::iterator insert(
        It pos, const typename Base::value_type &value) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (Base::size() >= max_size()) {
          throw MaxSizeException(
              "Destination has limited size by {}; Size is already {} ",
              max_size(),
              Base::size());
        }
      }
      return Base::insert(std::move(pos), value);
    }

    /**
     * @brief Inserts multiple copies of value at a position.
     * @param pos Iterator to insert position
     * @param size Number of elements to insert
     * @param value Value to insert
     * @return Iterator to inserted range start
     * @throws MaxSizeException if result would exceed MaxSize.
     */
    template <typename It, typename... Args>
      requires Iter<Base, It>
    typename Base::iterator insert(It pos,
        typename Base::size_type size,
        const typename Base::value_type &value) {
      if constexpr (size_check_is_enabled) {
        const auto available = max_size() - Base::size();
        [[unlikely]] if (available < size) {
          throw MaxSizeException(
              "Destination has limited size by {}; Requested size is {}",
              max_size(),
              size);
        }
      }
      return Base::insert(std::move(pos), size, value);
    }

    /**
     * @brief Inserts a range at a position.
     * @tparam OutIt Output iterator
     * @tparam InIt Input iterator
     * @param pos Position to insert at
     * @param begin Range begin
     * @param end Range end
     * @return Iterator to first inserted element
     * @throws MaxSizeException if result would exceed MaxSize.
     */
    template <typename OutIt, typename InIt>
      requires Iter<Base, OutIt>
        and std::is_base_of_v<std::input_iterator_tag,
            typename std::iterator_traits<InIt>::iterator_category>
    typename Base::iterator insert(OutIt pos, InIt begin, InIt end) {
      if constexpr (size_check_is_enabled) {
        const size_t size = std::distance(begin, end);
        const auto available = max_size() - Base::size();
        [[unlikely]] if (available < size) {
          throw MaxSizeException(
              "Destination has limited size by {} and current size is {}; "
              "Source range size is {} and would overflow destination",
              max_size(),
              Base::size(),
              size);
        }
      }
      return Base::insert(std::move(pos), std::move(begin), std::move(end));
    }

    /**
     * @brief Inserts initializer list at a position.
     * @param pos Iterator to insert position
     * @param list Values to insert
     * @return Iterator to first inserted element
     * @throws MaxSizeException if result would exceed MaxSize.
     */
    template <typename It, typename... Args>
      requires Iter<Base, It>
    typename Base::iterator insert(
        It pos, std::initializer_list<typename Base::value_type> &&list) {
      if constexpr (size_check_is_enabled) {
        const auto available = max_size() - Base::size();
        [[unlikely]] if (available < list.size()) {
          throw MaxSizeException(
              "Container has limited size by {}; Source range size is {}",
              max_size(),
              Base::size());
        }
      }
      return Base::insert(pos, std::move(list));
    }

    /**
     * @brief Pushes value to the end of container.
     * @param value Value to push
     * @throws MaxSizeException if container is full.
     */
    template <typename V>
    void push_back(V &&value) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (Base::size() >= max_size()) {
          throw MaxSizeException(
              "Container has limited size by {}; Size is already maximum",
              max_size());
        }
      }
      Base::push_back(std::forward<V>(value));
    }

    /**
     * @brief Reserves storage for the specified number of elements.
     * @param size Number of elements to reserve space for
     * @throws MaxSizeException if size exceeds MaxSize
     */
    void reserve(typename Base::size_type size) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (size > max_size()) {
          throw MaxSizeException(
              "Destination has limited size by {}; Requested size is {}",
              max_size(),
              size);
        }
      }
      return Base::reserve(size);
    }

    /**
     * @brief Resizes the container to contain exactly @p size elements.
     * @param size New desired size
     * @throws MaxSizeException if size exceeds MaxSize
     */
    void resize(typename Base::size_type size) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (size > max_size()) {
          throw MaxSizeException(
              "Destination has limited size by {}; Requested size is {}",
              max_size(),
              size);
        }
      }
      return Base::resize(size);
    }

    /**
     * @brief Resizes the container and fills new elements with given value.
     * @param size New desired size
     * @param value Value to assign to new elements
     * @throws MaxSizeException if size exceeds MaxSize
     */
    void resize(
        typename Base::size_type size, const typename Base::value_type &value) {
      if constexpr (size_check_is_enabled) {
        [[unlikely]] if (size > max_size()) {
          throw MaxSizeException(
              "Destination has limited size by {}; Requested size is {}",
              max_size(),
              size);
        }
      }
      return Base::resize(size, value);
    }

    /**
     * @brief Lexicographical comparison with another container.
     * @param other Container to compare with
     * @return Comparison result (three-way comparison operator)
     */
    auto operator<=>(const SizeLimitedContainer &other) const {
      return qtils::cxx20::lexicographical_compare_three_way(
          Base::begin(), Base::end(), other.begin(), other.end());
    }
  };

  /**
   * @brief Alias for size-limited std::vector.
   * @tparam ElementType Type of elements
   * @tparam MaxSize Maximum number of elements
   * @tparam Allocator Allocator type (default: std::allocator)
   */
  template <typename ElementType,
      size_t MaxSize,
      typename Allocator = std::allocator<ElementType>>
  using SLVector =
      SizeLimitedContainer<std::vector<ElementType, Allocator>, MaxSize>;

}  // namespace qtils
