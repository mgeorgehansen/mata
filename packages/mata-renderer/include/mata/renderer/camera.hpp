/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <mata/utils/propagate_const.hpp>

namespace mata {
namespace renderer {

class Camera final {
  class Impl;
  MATA_PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  explicit Camera();
  ~Camera() noexcept;

  void translateBy(const glm::vec2 &translation);
  [[nodiscard]] glm::mat4 viewMatrix() const;
};

} // namespace renderer
} // namespace mata
