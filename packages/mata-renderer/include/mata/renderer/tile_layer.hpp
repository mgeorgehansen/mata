/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <vector>

#include <mata/core/geometry.hpp>
#include <mata/core/types.hpp>
#include <mata/utils/propagate_const.hpp>

#include "texture.hpp"
#include "tileset.hpp"

namespace mata {
namespace renderer {

class TileLayer final {
  class Impl;
  MATA_PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  TileLayer(const mata::core::GridDimensions2d &dimensions,
            const Tileset &tileset,
            const std::vector<mata::core::Index2d> &tiles) noexcept;
  TileLayer(const mata::core::GridDimensions2d &dimensions,
            const Tileset &tileset) noexcept;
  ~TileLayer() noexcept;

  TileLayer(const TileLayer &other) noexcept;
  TileLayer &operator=(TileLayer &other) noexcept;
  friend void swap(TileLayer &first, TileLayer &second) noexcept;

  [[nodiscard]] mata::core::GridDimensions2d dimensions() const noexcept;

  [[nodiscard]] const Tileset &tileset() const noexcept;

  [[nodiscard]] mata::core::Index2d
  tileAt(const mata::core::Index2d &index) const noexcept;
};

} // namespace renderer
} // namespace mata
