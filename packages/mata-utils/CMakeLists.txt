# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")

add_library(mata-utils INTERFACE)
target_sources(mata-utils INTERFACE ${HEADERS})
target_include_directories(
  mata-utils INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_library(mata::utils ALIAS mata-utils)
