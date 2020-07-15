/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <functional>
#include <memory>

#include "concepts/noncopyable.hpp"
#include "utils/propagate_const.hpp"

namespace mata {

using ProcFunc = void (*)();

using GlProcAddressFunc = std::function<ProcFunc(const char *)>;

class [[nodiscard]] Window final : private noncopyable {
  class Impl;
  PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  explicit Window(const bool headless);
  ~Window() noexcept;

  GlProcAddressFunc glProcAddressFunc() const;

  void update();

  using ResizeCallback =
      std::function<void(const unsigned int width, const unsigned int height)>;

  void onResize(const ResizeCallback callback);

  using WindowCloseRequestedCallback = std::function<void()>;

  void onWindowCloseRequested(const WindowCloseRequestedCallback callback);

  using KeyEventCallback = std::function<void(const int key, const int action)>;

  void onKeyEvent(const KeyEventCallback callback);
};

} // namespace mata
