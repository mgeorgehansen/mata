# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

find_package(Catch2 CONFIG REQUIRED)

add_executable(mata_test test.cpp)
target_compile_features(mata_test PRIVATE cxx_std_17)

target_link_libraries(mata_test PRIVATE mata_lib Catch2::Catch2)

add_test(NAME mata_test COMMAND mata_test)
