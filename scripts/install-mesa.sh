#!/bin/bash

SRC_DIR="${SRC_DIR:-$HOME/src}"
MESA_VER=20.1.2
MESA_PKG="mesa-${MESA_VER}"

if [ ! -d "$SRC_DIR" ]; then
  mkdir -p "$SRC_DIR"
fi
cd "$SRC_DIR" || exit 1

if [ ! -d "${MESA_PKG}" ]; then
  echo "downloading mesa sources..."
  curl -L "https://archive.mesa3d.org/${MESA_PKG}.tar.xz" -o "${MESA_PKG}.tar.xz" || exit 1
  tar xf "${MESA_PKG}.tar.xz" || exit 1
  rm "${MESA_PKG}.tar.xz"
  echo "mesa sources unpacked."
fi

BUILD_DIR="build"

cd "${MESA_PKG}" || exit 1
if [ ! -d build/ ]; then
  echo "configuring mesa..."
  meson setup "${BUILD_DIR}/" \
    -Dc_std=c11 \
    -Dcpp_std=c++11 \
    -Dosmesa=gallium \
    -Dplatforms=surfaceless \
    -Dgles1=false \
    -Dgles2=false \
    -Dglx=disabled \
    -Degl=false \
    || exit 1
  echo "mesa configured."
fi

ninja -C "${BUILD_DIR}/"

echo 'installing...'
# KLUDGE: Fix for issue related to https://github.com/mesonbuild/meson/issues/2121.
# Installation seems to modify the original built .dylibs and rewrite the rpaths
# in the headers, causing subsequent installation failure on cached reruns. This
# works around the issue by backing up the build dir before install, ensuring
# that the backup is restored after install succeeds to preserve the original
# built files.
BACKUP_BUILD_DIR="${BUILD_DIR}-bck"
cp -r "${BUILD_DIR}" "${BACKUP_BUILD_DIR}" || exit 1
sudo meson install -C "${BUILD_DIR}/"
exit_code=$?
rm -rf "${BUILD_DIR:?}/" && mv "${BACKUP_BUILD_DIR}" "${BUILD_DIR}" || exit 1
if [ 0 -ne ${exit_code} ]; then
  echo 'installation failed.'
  exit 1
fi
echo 'installation succeeded.'
