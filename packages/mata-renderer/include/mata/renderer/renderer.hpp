/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <mata/platform/virtual_file_system.hpp>
#include <mata/utils/propagate_const.hpp>

#include "camera.hpp"
#include "tile_layer.hpp"
#include "window.hpp"

namespace mata {
namespace renderer {

class Renderer final {
  class Impl;
  MATA_PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  using LayerIdx = unsigned int;

  Renderer(const Window &window,
           const std::shared_ptr<mata::platform::VirtualFileSystem>);
  ~Renderer() noexcept;

  void setLayer(const LayerIdx layerN, const TileLayer &layer);

  void updateCamera(const Camera &camera) noexcept;

  void toggleWireframeMode();

  void drawFrame();

  void resize(const int width, const int height);
};

} // namespace renderer
} // namespace mata
