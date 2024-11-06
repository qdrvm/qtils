/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <filesystem>
#include <fstream>
#include <qtils/bytes.hpp>
#include <qtils/outcome.hpp>

namespace qtils {
  template <typename Out>
  outcome::result<void> readFile(Out &out, const std::filesystem::path &path) {
    std::ifstream file{path, std::ios::binary | std::ios::ate};
    if (not file.good()) {
      out.clear();
      return std::errc{errno};
    }
    out.resize(file.tellg());
    file.seekg(0);
    file.read(reinterpret_cast<char *>(out.data()), out.size());
    if (not file.good()) {
      out.clear();
      return std::errc{errno};
    }
    return outcome::success();
  }

  inline outcome::result<qtils::Bytes> readBytes(
      const std::filesystem::path &path) {
    qtils::Bytes out;
    OUTCOME_TRY(readFile(out, path));
    return out;
  }

  inline outcome::result<std::string> readText(
      const std::filesystem::path &path) {
    std::string out;
    OUTCOME_TRY(readFile(out, path));
    return out;
  }
}  // namespace qtils
