#!/bin/sh
# build.sh
meson setup build
meson compile -C build
mkdir -p dist
cp build/hello.so dist/hello.so