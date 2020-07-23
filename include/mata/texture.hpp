/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>

#include "geometry.hpp"
#include "types.hpp"
#include "utils/propagate_const.hpp"

namespace mata {

class Texture final {
  class Impl;
  PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  [[nodiscard]] static Texture fromPng(const bytes &png);

  explicit Texture(const GridDimensions2d &dimensions, const bytes &rgba);
  ~Texture() noexcept;

  Texture(const Texture &other) noexcept;
  Texture &operator=(Texture &other) noexcept;
  friend void swap(Texture &first, Texture &second) noexcept;

  [[nodiscard]] GridDimensions2d dimensions() const noexcept;

  [[nodiscard]] const bytes &asBytes() const noexcept;
};

} // namespace mata
