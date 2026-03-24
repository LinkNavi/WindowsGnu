#!/bin/sh
# build.sh
meson setup build
meson compile -C build
mkdir -p dist
cp build/WindowsGnu.so dist/WindowsGnu.so