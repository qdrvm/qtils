/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdio>
#include <format>
#include <iostream>

namespace qtils::cxx23 {

  template <class... Args>
  void print(std::format_string<Args...> fmt, Args &&...args) {
    print(stdout, fmt, std::forward<Args>(args)...);
  }

  template <class... Args>
  void print(
      std::FILE *stream, std::format_string<Args...> fmt, Args &&...args) {
    std::string str = std::format(fmt, std::forward<Args>(args)...);
    size_t size = fwrite(str.data(), 1, str.size(), stream);
    if (size < str.size()) {
      if (std::feof(stream)) {
        throw std::system_error(EIO, std::system_category(), "EOF while writing the formatted output");
      }
      throw std::system_error(
          std::ferror(stream), std::system_category(), "failed to write formatted output");
    }
  }

  template <class... Args>
  void print(
      std::ostream &os, std::format_string<Args...> fmt, Args &&...args) {
    os << std::vformat(os.getloc(), fmt.get(), std::make_format_args(args...));
  }

}  // namespace qtils::cxx23