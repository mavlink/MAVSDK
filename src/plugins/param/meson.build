param_include = include_directories(['include'])

# TODO: target properties
param = shared_library('mavsdk_param',
  ['param.cpp', 'param_impl.cpp'],
  include_directories: [core_include, mavlink_include, param_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [param],
  subdirs: ['mavsdk/plugins/param', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_param',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/param/param.h',
  subdir: 'mavsdk/plugins/param')
