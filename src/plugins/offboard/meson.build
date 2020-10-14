offboard_include = include_directories('include')

# TODO: target properties
offboard = shared_library('mavsdk_offboard',
  ['offboard.cpp', 'offboard_impl.cpp'],
  include_directories: [core_include, mavlink_include, offboard_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/offboard/offboard.h')
