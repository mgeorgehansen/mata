#!/bin/bash

mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="$VCPKG_HOME/scripts/buildsystems/vcpkg.cmake" .. || exit 1
cmake --build . || exit 1
ctest --output-on-failure || exit 1
