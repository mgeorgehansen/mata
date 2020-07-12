/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>

#include "concepts/noncopyable.hpp"
#include "layer.hpp"
#include "utils/propagate_const.hpp"
#include "virtual-file-system.hpp"

namespace mata {

struct Tile {
  glm::vec3 color;
};

class [[nodiscard]] Renderer final : private noncopyable {
  class Impl;
  PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  using LayerIdx = unsigned int;

  explicit Renderer(const std::shared_ptr<VirtualFileSystem>);
  ~Renderer() noexcept;

  void setLayer(const LayerIdx layerN, const Layer &layer);

  void updateViewMatrix(const glm::mat4 &viewMatrix);

  void drawFrame();

  void resize(const int width, const int height);
};

} // namespace mata
