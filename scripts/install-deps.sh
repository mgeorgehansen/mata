#!/bin/bash

ROOT_DIR=$(pwd)

git clone https://github.com/Microsoft/vcpkg.git &&
  cd vcpkg &&
  ./bootstrap-vcpkg.sh &&
  cd "$ROOT_DIR"

apt-get install -y libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev &&
  /opt/vcpkg/vcpkg install fmt glfw3 glbinding
