/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <algorithm>
#include <memory>
#include <utility>

#include <mata/core/geometry.hpp>

#include "mata/renderer/tileset.hpp"

namespace mata {
namespace renderer {

class Tileset::Impl final {
private:
  static const auto N_COLOR_CHANNELS = 4; // rgba

  static std::size_t
  nBytesPerTile(const mata::core::GridDimensions2d tileSize) {
    return static_cast<std::size_t>(tileSize.nColumns * tileSize.nRows *
                                    N_COLOR_CHANNELS);
  }

  mata::core::GridDimensions2d m_tileSize;
  mata::core::GridDimensions2d m_dimensions;
  Texture m_texture;

public:
  Impl(const mata::core::GridDimensions2d tileSize,
       const mata::core::GridDimensions2d dimensions,
       const Texture &texture) noexcept
      : m_tileSize(tileSize), m_dimensions(dimensions), m_texture(texture) {}

  mata::core::GridDimensions2d dimensions() const noexcept {
    return m_dimensions;
  }

  mata::core::GridDimensions2d tileSize() const noexcept { return m_tileSize; }

  const Texture &texture() const noexcept { return m_texture; }

  // When loaded from disk, the sprite atlas stores its bytes from left to
  // to right, top to bottom for the entire texture. We want to transpose the
  // pixel bytes so that we end up with all of our tiles vertically stacked in
  // order to make breaking them into layers easier in opengl:
  //
  // +--------+--------+       +--------+
  // | 111111 | 222222 |       | 111111 |
  // | 111111 | 222222 |       | 111111 |
  // |--------+--------|  ==>  +--------+
  // | 333333 | 444444 |       | 222222 |
  // | 333333 | 444444 |       | 222222 |
  // +--------+--------+       +--------+
  //                           | 333333 |
  //                           | 333333 |
  //                           +--------+
  //                           | 444444 |
  //                           | 444444 |
  //                           +--------+
  //
  // To do this we scan the pixels in the texture row by row, then copy the
  // individual rows in each tile within that row to an appropriate offset in
  // the dest texture:
  //
  // +--------+--------+       +--------+
  // |(111111)|(222222)|       |(111111)|
  // | 111111 | 222222 |       |        |
  // |--------+--------|  ==>  +--------+
  // | 333333 | 444444 |       |(222222)|
  // | 333333 | 444444 |       |        |
  // +--------+--------+       +--------+
  //                           |        |
  //                           |        |
  //                           +--------+
  //                           |        |
  //                           |        |
  //                           +--------+
  //                 ⬇
  // +--------+--------+       +--------+
  // | 111111 | 222222 |       | 111111 |
  // |(111111)|(222222)|       |(111111)|
  // |--------+--------|  ==>  +--------+
  // | 333333 | 444444 |       | 222222 |
  // | 333333 | 444444 |       |(222222)|
  // +--------+--------+       +--------+
  //                           |        |
  //                           |        |
  //                           +--------+
  //                           |        |
  //                           |        |
  //                           +--------+
  // ... etc.
  const mata::core::bytes asLinearBytes() const noexcept {
    const auto textureBytes = m_texture.asBytes();
    auto linearBytes = mata::core::bytes(textureBytes.size());

    const auto textureDims = m_texture.dimensions();
    const auto tilesetDims = m_dimensions;
    const auto nBytesPerTexturePxRow = textureDims.nColumns * N_COLOR_CHANNELS;
    const auto nBytesPerTilePxRow = m_tileSize.nColumns * N_COLOR_CHANNELS;
    const auto nPixelsPerTile = m_tileSize.nColumns * m_tileSize.nRows;
    const auto nBytesPerTile = nPixelsPerTile * N_COLOR_CHANNELS;
    for (auto pxRow = 0; pxRow < textureDims.nRows; pxRow++) {
      const auto rowBytesOffset = nBytesPerTexturePxRow * pxRow;

      // Subdivide each row of pixels in the texture into a row per tile.
      for (auto tileCol = 0; tileCol < tilesetDims.nColumns; tileCol++) {
        const auto srcStartOffset =
            rowBytesOffset + nBytesPerTilePxRow * tileCol;
        const auto srcEndOffset = srcStartOffset + nBytesPerTilePxRow;

        // Map the 2d index in the source to a 1d index in the dest.
        const auto tileRow = pxRow / m_tileSize.nRows;
        const auto destTileIndex = tileRow * tilesetDims.nColumns + tileCol;
        const auto tilePxRow = pxRow % m_tileSize.nColumns;
        const auto destOffset =
            (nBytesPerTile * destTileIndex) + (nBytesPerTilePxRow * tilePxRow);

        std::copy(textureBytes.begin() + srcStartOffset,
                  textureBytes.begin() + srcEndOffset,
                  linearBytes.begin() + destOffset);
      }
    }

    return linearBytes;
  }
};

Tileset::Tileset(const mata::core::GridDimensions2d tileSize,
                 const mata::core::GridDimensions2d dimensions,
                 const Texture &texture) noexcept
    : m_pImpl(std::make_unique<Impl>(tileSize, dimensions, texture)) {}

Tileset::~Tileset() noexcept = default;

Tileset::Tileset(const Tileset &other) noexcept
    : m_pImpl(new Impl(*other.m_pImpl)) {}
Tileset &Tileset::operator=(Tileset &other) noexcept {
  swap(*this, other);
  return *this;
};
void swap(Tileset &first, Tileset &second) noexcept {
  using std::swap;
  swap(first.m_pImpl, second.m_pImpl);
}

mata::core::GridDimensions2d Tileset::dimensions() const noexcept {
  return m_pImpl->dimensions();
}

mata::core::GridDimensions2d Tileset::tileSize() const noexcept {
  return m_pImpl->tileSize();
}

const Texture &Tileset::texture() const noexcept { return m_pImpl->texture(); }

mata::core::bytes Tileset::asLinearBytes() const noexcept {
  return m_pImpl->asLinearBytes();
}

} // namespace renderer
} // namespace mata
