/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <vector>

#include "concepts/noncopyable.hpp"
#include "utils/propagate_const.hpp"
#include "virtual-file-system.hpp"

namespace mata {

class [[nodiscard]] Renderer final : private noncopyable {
  class Impl;
  PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  explicit Renderer(const std::shared_ptr<VirtualFileSystem>);
  ~Renderer() noexcept;

  void startFrame() const;
  void pushGeometry(const std::vector<float> &vertices,
                    const std::vector<unsigned int> &indices) const;
  void endFrame() const;
  void resize(const int width, const int height) const;
};

} // namespace mata
