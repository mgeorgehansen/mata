# This Source Code Form is subject to the terms of the Mozilla Public License,
# v. 2.0. If a copy of the MPL was not distributed with this file, You can
# obtain one at https://mozilla.org/MPL/2.0/.

find_package(Catch2 CONFIG REQUIRED)

add_executable(smoke_test smoke.cpp)
target_compile_features(smoke_test PRIVATE cxx_std_17)

target_link_libraries(smoke_test PRIVATE mata_lib Catch2::Catch2)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME smoke_test COMMAND smoke_test)
