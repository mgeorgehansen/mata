/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>

#include "geometry.hpp"
#include "texture.hpp"
#include "utils/propagate_const.hpp"

namespace mata {

struct Tileset final {
  class Impl;
  PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  Tileset(const GridDimensions2d tileSize, const GridDimensions2d dimensions,
          const Texture &texture) noexcept;
  ~Tileset() noexcept;

  Tileset(const Tileset &other) noexcept;
  Tileset &operator=(Tileset &other) noexcept;
  friend void swap(Tileset &first, Tileset &second) noexcept;

  [[nodiscard]] GridDimensions2d dimensions() const noexcept;

  [[nodiscard]] GridDimensions2d tileSize() const noexcept;

  [[nodiscard]] const Texture &texture() const noexcept;

  [[nodiscard]] bytes asLinearBytes() const noexcept;
};

} // namespace mata
