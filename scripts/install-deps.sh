#!/bin/bash

VCPKG_DIR="$HOME/vcpkg"
VCPKG="$VCPKG_DIR/vcpkg"
export VCPKG_CMAKE_TOOLCHAIN="$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"

if [ ! -f "$VCPKG" ]; then
  cwd=$(pwd)
  git clone --depth=1 --branch 2020.06 https://github.com/Microsoft/vcpkg.git "$VCPKG_DIR" &&
    cd "$VCPKG_DIR" &&
    ./bootstrap-vcpkg.sh &&
    cd "$cwd"
fi

"$VCPKG" install fmt glfw3 glbinding
