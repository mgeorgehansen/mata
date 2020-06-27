#!/bin/bash

VCPKG_HOME="${VCPKG_HOME:-$HOME/vcpkg}"
VCPKG="$VCPKG_HOME/vcpkg"
echo "$0: using VCPKG_HOME=${VCPKG_HOME}"

cwd=$(pwd)
cd "$VCPKG_HOME"

if ! git rev-parse --is-inside-work-tree &> /dev/null; then
  echo "$0: vcpkg sources not found, cloning vcpkg repo..."
  git clone --depth=1 --branch 2020.06 https://github.com/Microsoft/vcpkg.git . || exit 1
fi

if [ ! -f "$VCPKG" ]; then
  echo "$0: vcpkg binary not found at $VCPKG, building..."
  ./bootstrap-vcpkg.sh || exit 1
fi

cd "$cwd"

"$VCPKG" install fmt glfw3 glbinding
