# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(mata ${HEADERS} ${SOURCES})
target_include_directories(mata PUBLIC "include/")
target_link_libraries(
  mata
  PRIVATE mata::core
          mata::platform
          mata::renderer
          mata::utils
          std::filesystem
          glfw
          fmt::fmt)
