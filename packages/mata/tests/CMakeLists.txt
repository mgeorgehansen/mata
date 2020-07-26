# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

find_package(Catch2 CONFIG REQUIRED)

get_target_property(MATA_SOURCE_DIR mata::lib SOURCE_DIR)
set(MATA_RESOURCES_PATH "${MATA_SOURCE_DIR}/resources")
configure_file(config.hpp.in config.hpp @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(smoke_test smoke.cpp)
target_compile_features(smoke_test PRIVATE cxx_std_17)
target_link_libraries(smoke_test PRIVATE mata::lib Catch2::Catch2)
add_test(NAME smoke_test COMMAND smoke_test)
