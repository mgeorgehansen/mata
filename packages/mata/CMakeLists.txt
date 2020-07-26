# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(mata-lib ${HEADERS} ${SOURCES})
target_include_directories(mata-lib PUBLIC "include/")
target_link_libraries(
  mata-lib
  PUBLIC mata::utils
  PRIVATE mata::core mata::platform mata::renderer std::filesystem glfw
          fmt::fmt)
add_library(mata::lib ALIAS mata-lib)

add_executable(mata "mata.cpp")
target_link_libraries(mata PRIVATE mata::lib mata::utils)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
