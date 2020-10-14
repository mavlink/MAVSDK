mavlink_passthrough_include = include_directories('include')

# TODO: target properties
mavlink_passthrough = shared_library('mavsdk_mavlink_passthrough',
  ['mavlink_passthrough.cpp', 'mavlink_passthrough_impl.cpp'],
  include_directories: [core_include, mavlink_include, mavlink_passthrough_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/mavlink_passthrough/mavlink_passthrough.h')
