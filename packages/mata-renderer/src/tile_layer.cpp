/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <memory>
#include <vector>

#include <mata/core/geometry.hpp>

#include "mata/renderer/tile_layer.hpp"
#include "mata/renderer/tileset.hpp"

namespace mata {
namespace renderer {

class TileLayer::Impl {
private:
  mata::core::GridDimensions2d m_dimensions;
  Tileset m_tileset;
  mata::core::GridContainer<mata::core::Index2d> m_tiles;

public:
  Impl(const mata::core::GridDimensions2d &dimensions,
       const Tileset &tileset) noexcept
      : m_dimensions(dimensions), m_tileset(tileset), m_tiles(dimensions) {}

  Impl(const mata::core::GridDimensions2d &dimensions, const Tileset &tileset,
       const std::vector<mata::core::Index2d> &tiles) noexcept
      : m_dimensions(dimensions), m_tileset(tileset),
        m_tiles(dimensions, tiles) {}

  mata::core::GridDimensions2d dimensions() const noexcept {
    return m_dimensions;
  }

  const Tileset &tileset() const noexcept { return m_tileset; }

  mata::core::Index2d tileAt(const mata::core::Index2d &index) const noexcept {
    return m_tiles.at(index);
  }
};

TileLayer::TileLayer(const mata::core::GridDimensions2d &dimensions,
                     const Tileset &tileset,
                     const std::vector<mata::core::Index2d> &tiles) noexcept
    : m_pImpl(std::make_unique<Impl>(dimensions, tileset, tiles)) {}

TileLayer::TileLayer(const mata::core::GridDimensions2d &dimensions,
                     const Tileset &tileset) noexcept
    : m_pImpl(std::make_unique<Impl>(dimensions, tileset)) {}

TileLayer::~TileLayer() noexcept = default;

TileLayer::TileLayer(const TileLayer &other) noexcept
    : m_pImpl(new Impl(*other.m_pImpl)) {}
TileLayer &TileLayer::operator=(TileLayer &other) noexcept {
  swap(*this, other);
  return *this;
}
void swap(TileLayer &first, TileLayer &second) noexcept {
  using std::swap;

  swap(first.m_pImpl, second.m_pImpl);
}

mata::core::GridDimensions2d TileLayer::dimensions() const noexcept {
  return m_pImpl->dimensions();
}

const Tileset &TileLayer::tileset() const noexcept {
  return m_pImpl->tileset();
}

mata::core::Index2d
TileLayer::tileAt(const mata::core::Index2d &index) const noexcept {
  return m_pImpl->tileAt(index);
}

} // namespace renderer
} // namespace mata
