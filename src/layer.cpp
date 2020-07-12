/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <memory>
#include <ostream>
#include <pprint/pprint.hpp>
#include <vector>

#include "mata/layer.hpp"

namespace mata {

std::ostream &operator<<(std::ostream &os, const Mesh2d &mesh) {
  auto printer = pprint::PrettyPrinter{os};
  printer.print("Mesh2d {");
  printer.indent(2);
  printer.print_inline("vertices:", mesh.vertices);
  printer.print_inline("indices:", mesh.indices);
  printer.print("}");
  return os;
}

bool operator==(const Mesh2d &mesh, const Mesh2d &other) {
  return mesh.vertices == other.vertices && mesh.indices == other.indices;
}

inline unsigned int nVerticesInGrid(const unsigned int width,
                                    const unsigned int height) {
  const auto nVertsPerRow = width + 1;
  const auto nRows = height + 1;
  return nVertsPerRow * nRows;
}

inline unsigned int nTrianglesInGrid(const unsigned int width,
                                     const unsigned int height) {
  const auto nCells = width * height;
  return 2 * nCells;
}

class [[nodiscard]] Layer::Impl final {
private:
  unsigned int m_width;
  unsigned int m_height;

public:
  Impl(unsigned int width, unsigned int height)
      : m_width(width), m_height(height) {}

  unsigned int width() const { return m_width; }

  unsigned int height() const { return m_height; }

  Mesh2d mesh() const {
    auto vertices = std::vector<float>();
    vertices.reserve(nVerticesInGrid(m_width, m_height) * 2);
    auto indices = std::vector<unsigned int>();
    indices.reserve(nTrianglesInGrid(m_width, m_height) * 3);

    // Create the first row of vertices.
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    for (unsigned int x = 0; x < m_width; x++) {
      vertices.push_back(static_cast<float>(x + 1));
      vertices.push_back(0.0f);
    }
    for (unsigned int y = 0; y < m_height; y++) {
      vertices.push_back(0.0f);
      vertices.push_back(static_cast<float>(y + 1));
      for (unsigned int x = 0; x < m_width; x++) {
        // (a)----(b)----(c) <--- assume these vertices already exist
        //  |      |      |
        //  |      |      |
        // (d)-----e------f <-- now we only need to add one vertex per cell
        //  ^
        //  Add this vertex at the start of the row.
        vertices.push_back(static_cast<float>(x + 1));
        vertices.push_back(static_cast<float>(y + 1));

        // Vertices are stored contiguously like this:
        // a-----b-----c
        //           /
        //        /
        //     /
        //  /
        // d-----e-----f
        //          ^ cell (x, y)
        //
        // ...which means that we can easily calc the index of a since it is a
        // function of the current column x and the total number of columns
        // in the row.
        const auto nVerticesPerRow = m_width + 1;
        const auto eIdx = ((y + 1) * nVerticesPerRow) + x;
        const auto fIdx = eIdx + 1;
        const auto bIdx = eIdx - nVerticesPerRow;
        const auto cIdx = bIdx + 1;

        // We want to make counter-clockwise triangles like this:
        // a------b------c
        //        | t1  /|
        //        |   /  |
        //        | / t2 |
        // d------e------f
        indices.push_back(bIdx);
        indices.push_back(eIdx);
        indices.push_back(cIdx);

        indices.push_back(eIdx);
        indices.push_back(fIdx);
        indices.push_back(cIdx);
      }
    }
    return {vertices, indices};
  }
};

Layer::Layer(unsigned int width, unsigned int height)
    : m_pImpl(std::make_unique<Impl>(width, height)) {}

Layer::~Layer() noexcept = default;

unsigned int Layer::width() const { return this->m_pImpl->width(); }

unsigned int Layer::height() const { return this->m_pImpl->height(); }

Mesh2d Layer::mesh() const { return this->m_pImpl->mesh(); }

} // namespace mata
