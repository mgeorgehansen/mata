# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")

add_library(mata-core INTERFACE)
target_sources(mata-core INTERFACE ${HEADERS})
target_include_directories(
  mata-core INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_library(mata::core ALIAS mata-core)
