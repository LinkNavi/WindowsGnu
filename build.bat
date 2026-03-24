rem build.bat
meson setup build
meson compile -C build
mkdir dist
copy build\hello.dll dist\hello.dll