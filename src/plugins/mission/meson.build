mission_include = include_directories(['include'])

# TODO: target properties
mission = shared_library('mavsdk_mission',
  ['mission.cpp', 'mission_impl.cpp'],
  include_directories: [core_include, mavlink_include, mission_include],
  link_with: mavsdk_lib,
  dependencies: [jsoncpp_dep, threads],
  install: true
)

pkg.generate(
  libraries: [mission],
  subdirs: ['mavsdk/plugins/mission', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_mission',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/mission/mission.h',
  subdir: 'mavsdk/plugins/mission')
