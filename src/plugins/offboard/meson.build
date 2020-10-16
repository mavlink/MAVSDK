offboard_include = include_directories(['include'])

# TODO: target properties
offboard = shared_library('mavsdk_offboard',
  ['offboard.cpp', 'offboard_impl.cpp'],
  include_directories: [core_include, mavlink_include, offboard_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [offboard],
  subdirs: ['mavsdk/plugins/offboard', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_offboard',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/offboard/offboard.h',
  subdir: 'mavsdk/plugins/offboard')
