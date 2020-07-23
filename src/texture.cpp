/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <fmt/format.h>
#include <lodepng.h>
#include <memory>
#include <stdexcept>
#include <utility>

#include <mata/geometry.hpp>
#include <mata/texture.hpp>
#include <mata/types.hpp>

namespace mata {

class Texture::Impl {
private:
  GridDimensions2d m_dimensions;
  bytes m_rgba;

public:
  Impl(const GridDimensions2d &dimensions, const bytes &rgba) noexcept
      : m_dimensions(dimensions), m_rgba(rgba) {}

  GridDimensions2d dimensions() const noexcept { return m_dimensions; }

  const bytes &asBytes() const noexcept { return m_rgba; }
};

Texture Texture::fromPng(const bytes &png) {
  auto rgba = bytes{};
  unsigned int width, height;
  const auto error = lodepng::decode(rgba, width, height, png);
  if (error) {
    throw std::runtime_error(fmt::format(
        "failed to decode PNG as RGBA texture: {}", lodepng_error_text(error)));
  }
  return Texture({static_cast<int>(width), static_cast<int>(height)}, rgba);
}

Texture::Texture(const GridDimensions2d &dimensions, const bytes &rgba)
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

GridDimensions2d Texture::dimensions() const noexcept {
  return m_pImpl->dimensions();
}

const bytes &Texture::asBytes() const noexcept { return m_pImpl->asBytes(); }

} // namespace mata
