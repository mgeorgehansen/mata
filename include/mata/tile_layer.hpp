/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <vector>

#include "geometry.hpp"
#include "texture.hpp"
#include "tileset.hpp"
#include "utils/propagate_const.hpp"

namespace mata {

class TileLayer final {
  class Impl;
  PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  TileLayer(const GridDimensions2d &dimensions, const Tileset &tileset,
            const std::vector<Index2d> &tiles) noexcept;
  TileLayer(const GridDimensions2d &dimensions,
            const Tileset &tileset) noexcept;
  ~TileLayer() noexcept;

  TileLayer(const TileLayer &other) noexcept;
  TileLayer &operator=(TileLayer &other) noexcept;
  friend void swap(TileLayer &first, TileLayer &second) noexcept;

  [[nodiscard]] GridDimensions2d dimensions() const noexcept;

  [[nodiscard]] const Tileset &tileset() const noexcept;

  [[nodiscard]] Index2d tileAt(const Index2d &index) const noexcept;
};

} // namespace mata
