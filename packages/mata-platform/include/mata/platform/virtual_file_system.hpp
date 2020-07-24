/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include <mata/core/types.hpp>
#include <mata/utils/propagate_const.hpp>

namespace mata {
namespace platform {

class VirtualFileSystem final {
  class Impl;
  MATA_PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  explicit VirtualFileSystem(const std::filesystem::path &rootPath);
  ~VirtualFileSystem() noexcept;

  [[nodiscard]] mata::core::bytes
  readFile(const std::filesystem::path &path) const;
  [[nodiscard]] std::string
  readTextFile(const std::filesystem::path &path) const;
};

} // namespace platform
} // namespace mata
