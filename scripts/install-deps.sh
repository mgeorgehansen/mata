#!/bin/bash

VCPKG_DIR="$TRAVIS_BUILD_DIR/vcpkg"
VCPKG="$VCPKG_DIR/vcpkg"
export VCPKG_CMAKE_TOOLCHAIN="$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"

if [ ! -f "$VCPKG" ]; then
  git clone --depth=1 --branch 2020.06 https://github.com/Microsoft/vcpkg.git "$VCPKG_DIR" &&
    cd "$VCPKG_DIR" &&
    ./bootstrap-vcpkg.sh
fi

"$VCPKG" install fmt glfw3 glbinding
