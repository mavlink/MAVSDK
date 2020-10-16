mission_raw_include = include_directories(['include'])

# TODO: target properties
mission_raw = shared_library('mavsdk_mission_raw',
  ['mission_raw.cpp', 'mission_raw_impl.cpp'],
  include_directories: [core_include, mavlink_include, mission_raw_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [mission_raw],
  subdirs: ['mavsdk/plugins/mission_raw', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_mission_raw',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/mission_raw/mission_raw.h',
  subdir: 'mavsdk/plugins/mission_raw')
