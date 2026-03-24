# meson.build
project('hello', 'cpp', default_options: ['cpp_std=c++17'])
include_dir = include_directories('include')
shared_library('hello',
    'plugin.cpp',
    name_prefix: '',
    include_directories: include_dir,
    install: true,
    install_dir: 'dist'
)