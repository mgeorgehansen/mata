#!/bin/bash

ROOT_DIR=$(pwd)

git clone https://github.com/Microsoft/vcpkg.git &&
  cd vcpkg &&
  ./bootstrap-vcpkg.sh &&
  vcpkg install fmt glfw3 glbinding
