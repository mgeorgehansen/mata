/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <glm/vec2.hpp>
#include <vector>

namespace mata {

using Coord2d = glm::vec2;

struct Index2d {
  int i;
  int j;
};

struct Dimensions2d {
  float width;
  float height;
};

struct GridDimensions2d {
  int nColumns;
  int nRows;
};

inline int index2dTo1d(const Index2d &index,
                       const GridDimensions2d &dimensions) {
  return dimensions.nColumns * index.j + index.i;
}

struct Rect {
public:
  Coord2d topLeft;
  Coord2d bottomLeft;
  Coord2d bottomRight;
  Coord2d topRight;

  constexpr Rect(const Coord2d &topLeft_, const Coord2d &bottomLeft_,
                 const Coord2d &bottomRight_, const Coord2d &topRight_) noexcept
      : topLeft(topLeft_), bottomLeft(bottomLeft_), bottomRight(bottomRight_),
        topRight(topRight_) {}
  constexpr Rect(const Coord2d &position,
                 const Dimensions2d &dimensions) noexcept
      : Rect(position, {position.x, position.y + dimensions.height},
             {position.x + dimensions.width, position.y + dimensions.height},
             {position.x + dimensions.width, position.y}) {}
};

template <typename T> class GridContainer {
private:
  GridDimensions2d m_dimensions;
  std::vector<T> m_elements;

  constexpr std::size_t index1d(const Index2d &index) const noexcept {
    return static_cast<std::size_t>(index2dTo1d(index, m_dimensions));
  }

  constexpr bool indexInRange(const Index2d &index) const noexcept {
    return index.i >= 0 && index.j >= 0 && index.i < m_dimensions.nColumns &&
           index.j < m_dimensions.nRows;
  }

public:
  constexpr explicit GridContainer(const GridDimensions2d &dimensions) noexcept
      : m_dimensions(dimensions),
        m_elements(
            static_cast<std::size_t>(dimensions.nColumns * dimensions.nRows)) {}

  constexpr GridContainer(const GridDimensions2d &dimensions,
                          const std::vector<T> &elements) noexcept
      : GridContainer(dimensions) {
    m_elements.assign(elements.begin(), elements.end());
  }

  constexpr void set(const Index2d &index, const T &value) noexcept {
    assert(indexInRange(index));

    m_elements[index1d(index)] = value;
  }

  constexpr const T &at(const Index2d &index) const noexcept {
    assert(indexInRange(index));

    return m_elements.at(index1d(index));
  }
};

} // namespace mata
