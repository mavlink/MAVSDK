shell_include = include_directories(['include'])

# TODO: target properties
shell = shared_library('mavsdk_shell',
  ['shell.cpp', 'shell_impl.cpp'],
  include_directories: [core_include, mavlink_include, shell_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [shell],
  subdirs: ['mavsdk/plugins/shell', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_shell',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/shell/shell.h',
  subdir: 'mavsdk/plugins/shell')
