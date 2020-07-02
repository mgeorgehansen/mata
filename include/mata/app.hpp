/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <filesystem>
#include <memory>
#include <optional>

#include "concepts/noncopyable.hpp"
#include "utils/propagate_const.hpp"

namespace mata {

struct AppParams {
  bool headless = false;
  std::optional<std::filesystem::path> resourcesPath = {};
};

class [[nodiscard]] App final : private noncopyable {
  class Impl;
  PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  App(const AppParams &params = AppParams{});
  ~App() noexcept;

  void stepFrame() const;
  void run() const;
};

} // namespace mata
