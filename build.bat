rem build.bat
meson setup build
meson compile -C build
mkdir dist
copy build\WindowsGnu.dll dist\WindowsGnu.dll