/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>

#include <mata/camera.hpp>
#include <mata/concepts/noncopyable.hpp>
#include <mata/utils/propagate_const.hpp>

namespace mata {

class [[nodiscard]] Camera::Impl final : private noncopyable {
  glm::mat4 m_transform = glm::mat4(1.0f);

public:
  void translateBy(const glm::vec2 &translation) {
    this->m_transform =
        glm::translate(this->m_transform, glm::vec3(translation, 0.0f));
  }

  glm::mat4 viiewMatrix() const { return this->m_transform; }
};

Camera::Camera() : m_pImpl(std::make_unique<Impl>()) {}

Camera::~Camera() noexcept = default;

void Camera::translateBy(const glm::vec2 &translation) {
  this->m_pImpl->translateBy(translation);
}

glm::mat4 Camera::viewMatrix() const { return this->m_pImpl->viiewMatrix(); }

} // namespace mata
