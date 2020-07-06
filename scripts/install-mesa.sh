#!/bin/bash

SRC_DIR="${SRC_DIR:-$HOME/src}"
MESA_VER=20.1.2
MESA_PKG="mesa-${MESA_VER}"

if [ ! -d "$SRC_DIR" ]; then
  mkdir -p "$SRC_DIR"
fi
cd "$SRC_DIR"

if [ ! -d "${MESA_PKG}" ]; then
  echo "downloading mesa sources..."
  curl -L "https://archive.mesa3d.org/${MESA_PKG}.tar.xz" -o "${MESA_PKG}.tar.xz" || exit 1
  tar xf "${MESA_PKG}.tar.xz" || exit 1
  rm "${MESA_PKG}.tar.xz"
  echo "mesa sources unpacked."
fi

cd "${MESA_PKG}"
echo "configuring mesa..."
meson setup build/ \
  -Dc_std=c11 \
  -Dcpp_std=c++11 \
  -Dosmesa=gallium \
  -Dplatforms=surfaceless \
  -Dgles1=false \
  -Dgles2=false \
  -Dglx=disabled \
  -Degl=false
echo "mesa configured."

sudo meson install -C build/ || exit 1
