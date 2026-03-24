# meson.build
project('WindowsGnu', 'cpp', default_options: ['cpp_std=c++17'])
src = files(
    'src/main.cpp',
)
include_dir = include_directories('include')
shared_library('WindowsGnu',
    src,
    name_prefix: '',
    include_directories: include_dir,
    install: true,
    install_dir: 'dist'
)