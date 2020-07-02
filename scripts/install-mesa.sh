#!/bin/bash

SRC_DIR="$HOME/src"
MESA_VER=20.1.2
MESA_PKG="mesa-${MESA_VER}"

cd "$SRC_DIR"
curl -L "https://archive.mesa3d.org/${MESA_PKG}.tar.xz" -o "${MESA_PKG}.tar.xz"
tar xf "${MESA_PKG}.tar.xz"
cd "${MESA_VER}"
meson setup build/ \
  -Dc_std=c17 \
  -Dcpp_std=c++17 \
  -Dosmesa=gallium \
  -Dplatforms=surfaceless \
  -Dgles1=false \
  -Dgles2=false \
  -Dglx=disabled \
  -Degl=false
meson compile -C build/
sudo meson install -C build/
