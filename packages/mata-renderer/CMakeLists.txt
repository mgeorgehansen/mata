# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(mata-renderer ${HEADERS} ${SOURCES})
add_library(mata::renderer ALIAS mata-renderer)
target_include_directories(mata-renderer PUBLIC "include/")
target_link_libraries(
  mata-renderer
  PRIVATE mata::core
          mata::utils
          mata::platform
          std::filesystem
          glbinding::glbinding
          glbinding::glbinding-aux
          fmt::fmt
          glm
          lodepng)
