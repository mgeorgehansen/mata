/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <experimental/propagate_const>
#include <memory>

#include "concepts/noncopyable.hpp"

namespace mata {

class [[nodiscard]] App final : private noncopyable {
  class Impl;
  std::experimental::propagate_const<std::unique_ptr<Impl>> m_pImpl;

public:
  App();
  ~App() noexcept;

  void run() const;
};

} // namespace mata
