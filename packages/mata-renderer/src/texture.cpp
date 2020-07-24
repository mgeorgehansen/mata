/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <memory>
#include <stdexcept>
#include <utility>

#include <fmt/format.h>
#include <lodepng.h>

#include <mata/core/geometry.hpp>
#include <mata/core/types.hpp>

#include "mata/renderer/texture.hpp"

namespace mata {
namespace renderer {

class Texture::Impl {
private:
  mata::core::GridDimensions2d m_dimensions;
  mata::core::bytes m_rgba;

public:
  Impl(const mata::core::GridDimensions2d &dimensions,
       const mata::core::bytes &rgba) noexcept
      : m_dimensions(dimensions), m_rgba(rgba) {}

  mata::core::GridDimensions2d dimensions() const noexcept {
    return m_dimensions;
  }

  const mata::core::bytes &asBytes() const noexcept { return m_rgba; }
};

Texture Texture::fromPng(const mata::core::bytes &png) {
  auto rgba = mata::core::bytes{};
  unsigned int width, height;
  const auto error = lodepng::decode(rgba, width, height, png);
  if (error) {
    throw std::runtime_error(fmt::format(
        "failed to decode PNG as RGBA texture: {}", lodepng_error_text(error)));
  }
  return Texture({static_cast<int>(width), static_cast<int>(height)}, rgba);
}

Texture::Texture(const mata::core::GridDimensions2d &dimensions,
                 const mata::core::bytes &rgba)
    : m_pImpl(std::make_unique<Impl>(dimensions, rgba)) {}
Texture::~Texture() noexcept = default;

Texture::Texture(const Texture &texture) noexcept
    : m_pImpl(new Impl(*texture.m_pImpl)) {}
Texture &Texture::operator=(Texture &other) noexcept {
  if (this != &other) {
    swap(*this, other);
  }
  return *this;
}
void swap(Texture &first, Texture &second) noexcept {
  using std::swap;

  swap(first.m_pImpl, second.m_pImpl);
}

mata::core::GridDimensions2d Texture::dimensions() const noexcept {
  return m_pImpl->dimensions();
}

const mata::core::bytes &Texture::asBytes() const noexcept {
  return m_pImpl->asBytes();
}

} // namespace renderer
} // namespace mata
