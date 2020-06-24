/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <experimental/propagate_const>
#include <filesystem>
#include <memory>
#include <string>

#include "concepts/noncopyable.hpp"

namespace mata {

class [[nodiscard]] VirtualFileSystem final : private noncopyable {
  class Impl;
  std::experimental::propagate_const<std::unique_ptr<Impl>> m_pImpl;

public:
  explicit VirtualFileSystem(const std::filesystem::path &rootPath);
  ~VirtualFileSystem() noexcept;

  [[nodiscard]] std::string readFile(const std::filesystem::path &path) const;
};

} // namespace mata
