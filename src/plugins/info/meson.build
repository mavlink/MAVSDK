info_include = include_directories(['include'])

# TODO: target properties
info = shared_library('mavsdk_info',
  ['info.cpp', 'info_impl.cpp'],
  include_directories: [core_include, mavlink_include, info_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [info],
  subdirs: ['mavsdk/plugins/info', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_info',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/info/info.h',
  subdir: 'mavsdk/plugins/info')
