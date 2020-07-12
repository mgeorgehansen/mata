/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include "utils/propagate_const.hpp"

namespace mata {

struct [[nodiscard]] Mesh2d {
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
};

bool operator==(const Mesh2d &mesh, const Mesh2d &other);
std::ostream &operator<<(std::ostream &os, const Mesh2d &mesh);

class [[nodiscard]] Layer final {
  class Impl;
  PROPAGATE_CONST(std::unique_ptr<Impl>) m_pImpl;

public:
  Layer(unsigned int width, unsigned int height);
  ~Layer() noexcept;

  unsigned int width() const;
  unsigned int height() const;

  Mesh2d mesh() const;
};

} // namespace mata
