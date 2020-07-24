/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>

#include <mata/core/geometry.hpp>
#include <mata/core/types.hpp>
#include <mata/utils/propagate_const.hpp>

#include "texture.hpp"

namespace mata {
namespace renderer {

struct Tileset final {
  class Impl;
  MATA_PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  Tileset(const mata::core::GridDimensions2d tileSize,
          const mata::core::GridDimensions2d dimensions,
          const Texture &texture) noexcept;
  ~Tileset() noexcept;

  Tileset(const Tileset &other) noexcept;
  Tileset &operator=(Tileset &other) noexcept;
  friend void swap(Tileset &first, Tileset &second) noexcept;

  [[nodiscard]] mata::core::GridDimensions2d dimensions() const noexcept;

  [[nodiscard]] mata::core::GridDimensions2d tileSize() const noexcept;

  [[nodiscard]] const Texture &texture() const noexcept;

  [[nodiscard]] mata::core::bytes asLinearBytes() const noexcept;
};

} // namespace renderer
} // namespace mata
