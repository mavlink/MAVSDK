action_include = include_directories(['include'])

# TODO: target properties
action = shared_library('mavsdk_action',
  ['action.cpp', 'action_impl.cpp'],
  include_directories: [core_include, mavlink_include, action_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [action],
  subdirs: ['mavsdk/plugins/action', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_action',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/action/action.h',
  subdir: 'mavsdk/plugins/action')
