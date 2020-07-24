/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>

#include <mata/core/geometry.hpp>
#include <mata/core/types.hpp>
#include <mata/utils/propagate_const.hpp>

namespace mata {
namespace renderer {

class Texture final {
  class Impl;
  MATA_PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  [[nodiscard]] static Texture fromPng(const mata::core::bytes &png);

  explicit Texture(const mata::core::GridDimensions2d &dimensions,
                   const mata::core::bytes &rgba);
  ~Texture() noexcept;

  Texture(const Texture &other) noexcept;
  Texture &operator=(Texture &other) noexcept;
  friend void swap(Texture &first, Texture &second) noexcept;

  [[nodiscard]] mata::core::GridDimensions2d dimensions() const noexcept;

  [[nodiscard]] const mata::core::bytes &asBytes() const noexcept;
};

} // namespace renderer
} // namespace mata
