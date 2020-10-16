mavlink_passthrough_include = include_directories(['include'])

# TODO: target properties
mavlink_passthrough = shared_library('mavsdk_mavlink_passthrough',
  ['mavlink_passthrough.cpp', 'mavlink_passthrough_impl.cpp'],
  include_directories: [core_include, mavlink_include, mavlink_passthrough_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [mavlink_passthrough],
  subdirs: ['mavsdk/plugins/mavlink_passthrough', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_mavlink_passthrough',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/mavlink_passthrough/mavlink_passthrough.h',
  subdir: 'mavsdk/plugins/mavlink_passthrough')
