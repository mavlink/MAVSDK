mocap_include = include_directories(['include'])

# TODO: target properties
mocap = shared_library('mavsdk_mocap',
  ['mocap.cpp', 'mocap_impl.cpp'],
  include_directories: [core_include, mavlink_include, mocap_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [mocap],
  subdirs: ['mavsdk/plugins/mocap', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_mocap',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/mocap/mocap.h',
  subdir: 'mavsdk/plugins/mocap')
