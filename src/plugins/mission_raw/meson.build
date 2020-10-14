mission_raw_include = include_directories('include')

# TODO: target properties
mission_raw = shared_library('mavsdk_mission_raw',
  ['mission_raw.cpp', 'mission_raw_impl.cpp'],
  include_directories: [core_include, mavlink_include, mission_raw_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/mission_raw/mission_raw.h')
