# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(mata-platform ${HEADERS} ${SOURCES})
target_include_directories(mata-platform PUBLIC "include/")
target_link_libraries(mata-platform PRIVATE mata::core mata::utils
                                            std::filesystem fmt::fmt)

add_library(mata::platform ALIAS mata-platform)
