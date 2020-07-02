#!/bin/bash

SRC_DIR="${SRC_DIR:-$HOME/src}"
MESA_VER=20.1.2
MESA_PKG="mesa-${MESA_VER}"

cd "$SRC_DIR" || exit 1
curl -L "https://archive.mesa3d.org/${MESA_PKG}.tar.xz" -o "${MESA_PKG}.tar.xz" || exit 1
tar xf "${MESA_PKG}.tar.xz" || exit 1
cd "${MESA_PKG}" || exit 1
meson setup build/ \
  -Dc_std=c11 \
  -Dcpp_std=c++11 \
  -Dosmesa=gallium \
  -Dplatforms=surfaceless \
  -Dgles1=false \
  -Dgles2=false \
  -Dglx=disabled \
  -Degl=false \
  || exit 1
meson compile -C build/ || exit 1
sudo meson install -C build/ || exit 1
