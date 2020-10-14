mission_include = include_directories('include')

# TODO: target properties
mission = shared_library('mavsdk_mission',
  ['mission.cpp', 'mission_impl.cpp'],
  include_directories: [core_include, mavlink_include, mission_include],
  link_with: mavsdk_lib,
  dependencies: [jsoncpp_dep, tinyxml2_dep, thread_dep],
  install: true
)

install_headers('include/plugins/mission/mission.h')
