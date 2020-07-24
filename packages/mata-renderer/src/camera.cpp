/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <memory>

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <mata/utils/propagate_const.hpp>

#include "mata/renderer/camera.hpp"

namespace mata {
namespace renderer {

class Camera::Impl final {
  glm::mat4 m_transform = glm::mat4(1.0f);

public:
  void translateBy(const glm::vec2 &translation) {
    this->m_transform =
        glm::translate(this->m_transform, glm::vec3(translation, 0.0f));
  }

  [[nodiscard]] glm::mat4 viiewMatrix() const { return this->m_transform; }
};

Camera::Camera() : m_pImpl(std::make_unique<Impl>()) {}

Camera::~Camera() noexcept = default;

void Camera::translateBy(const glm::vec2 &translation) {
  this->m_pImpl->translateBy(translation);
}

glm::mat4 Camera::viewMatrix() const { return this->m_pImpl->viiewMatrix(); }

} // namespace renderer
} // namespace mata
