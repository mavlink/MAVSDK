manual_control_include = include_directories('include')

# TODO: target properties
manual_control = shared_library('mavsdk_manual_control',
  ['manual_control.cpp', 'manual_control_impl.cpp'],
  include_directories: [core_include, mavlink_include, manual_control_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/manual_control/manual_control.h')
