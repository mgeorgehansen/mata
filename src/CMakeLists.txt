# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS
     "${PROJECT_SOURCE_DIR}/include/mata/*.hpp")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

add_library(mata_lib ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(mata_lib PUBLIC ../include)
target_link_libraries(
  mata_lib
  PRIVATE std::filesystem
          glfw
          glbinding::glbinding
          glbinding::glbinding-aux
          fmt::fmt
          glm
          pprint::pprint
          lodepng)
