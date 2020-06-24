#!/bin/bash

VCPKG_DIR="$TRAVIS_BUILD_DIR/vcpkg"

if [ ! -d ./vcpkg ]; then
  git clone --depth=1 --branch 2020.06 https://github.com/Microsoft/vcpkg.git "$VCPKG_DIR" &&
    cd "$VCPKG_DIR" &&
    ./bootstrap-vcpkg.sh
fi

"$VCPKG_DIR"/vcpkg install fmt glfw3 glbinding
