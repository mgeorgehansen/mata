#!/bin/bash

VCPKG_HOME="${VCPKG_HOME:-$HOME/vcpkg}"
VCPKG="$VCPKG_HOME/vcpkg"

if [ ! -f "$VCPKG" ]; then
  cwd=$(pwd)
  git clone --depth=1 --branch 2020.06 https://github.com/Microsoft/vcpkg.git "$VCPKG_DIR" &&
    cd "$VCPKG_HOME" &&
    ./bootstrap-vcpkg.sh &&
    cd "$cwd"
fi

"$VCPKG" install fmt glfw3 glbinding
